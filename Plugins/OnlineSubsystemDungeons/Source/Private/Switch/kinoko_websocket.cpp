#include <cassert>
#include <chrono>
#include <deque>
#include <httpClient/httpClient.h>
#include <httpClient/httpProvider.h>
#include <memory>
#include <mutex>
#include <nn/nifm.h>
#include <nn/os.h>
#include <nn/util/util_ScopeExit.h>
#include <nn/websocket.h>
#include <string>
#include <vector>

HC_DEFINE_TRACE_AREA(HC_WS, HCTraceLevel::Important);

namespace WebSocket
{

namespace
{

class Connection;

}

class Context
{
public:
    using Connections = std::vector<std::shared_ptr<Connection>>;

    void Register(std::unique_ptr<Connection>&& connection);
    void Unregister(Connection& connection);

    void PollAll() const;

    std::shared_ptr<Connection> Get(HCWebsocketHandle hcHandle) const noexcept;

private:
    std::unique_lock<std::mutex> Lock() const noexcept;

    mutable std::mutex m_mutex;

    Connections m_connections;
};

namespace
{

enum class RequestKind
{
    None,
    Connect,
    SendBinary,
    SendText,
    Disconnect,
};

#if HC_TRACE_VERBOSE_ENABLE
char const* StringFromKind(RequestKind k) noexcept
{
    static char const* const s[] = { "None", "Connect", "SendBinary", "SendText", "Disconnect" };
    return s[static_cast<size_t>(k)];
}
#endif

struct Request
{
    static Request Connect(XAsyncBlock* async);
    static Request SendBinary(XAsyncBlock* async, size_t size, uint8_t const* data);
    static Request SendText(XAsyncBlock* async, char const* text);
    static Request Disconnect();

    XAsyncBlock* OuterAsync = nullptr;
    XAsyncBlock* InnerAsync = nullptr;
    std::vector<uint8_t> Data;
    RequestKind Kind = RequestKind::None;
    nn::Result Result;
};

enum class ConnectionStatus
{
    Created,
    Connecting,
    Connected,
    Closing,
    Closed,
};

class Connection
{
public:
    Connection(
        Context* WebsocketContext,
        HCWebsocketHandle hc,
        XAsyncBlock* async,
        char const* uri,
        char const* subProtocol
    ) noexcept;

    ~Connection() noexcept;

    HCWebsocketHandle HcHandle() const noexcept;

    HRESULT Enqueue(Request&& request) noexcept;

    void Poll() noexcept;

private:
    std::unique_lock<std::mutex> Lock() const noexcept;

    HRESULT RunNextIfPossible() noexcept;
    void RegisterInnerAsync(XAsyncBlock* inner) noexcept;
    void StartRequest(std::unique_lock<std::mutex>&& lock) noexcept;
    void Complete(HRESULT hr, nn::Result res, bool withPayload, std::unique_lock<std::mutex>&& lock) noexcept;

    static HRESULT WriteProvider(XAsyncOp op, XAsyncProviderData const* data) noexcept;

    static void OnOpen(void* ctx) noexcept;
    static void OnMessage(void* data, size_t size, bool isText, void* ctx) noexcept;
    static void OnClose(uint16_t code, char const* reason, size_t size, void* ctx) noexcept;
    static void OnError(nn::Result res, void* ctx) noexcept;
    static bool OnCancellationCheck(void* ctx) noexcept;

    mutable std::mutex m_mutex;

    Context* const m_WebsocketContext;
    HCWebsocketHandle const m_hcHandle;
    std::string const m_uri;
    std::string const m_subProtocol;

    std::deque<Request> m_pending;
    std::vector<Request> m_completed;

    // only accessed from the task queue
    ConnectionStatus m_state;
    nn::websocket::Connection m_connection;
    nn::websocket::WebSocket m_socket;
    Request m_running;
};

}

void Context::Register(std::unique_ptr<Connection>&& connection)
{
    auto lock = Lock();
    m_connections.emplace_back(std::move(connection));
}

void Context::Unregister(Connection& connection)
{
    auto lock = Lock();
    m_connections.erase(std::remove_if(
        m_connections.begin(),
        m_connections.end(),
        [&](auto const& item) { return item.get() == &connection; }
    ), m_connections.end());
}

void Context::PollAll() const
{
    auto lock = Lock();
    auto connections = m_connections;
    lock.unlock();

    for (auto const& c : connections)
    {
        c->Poll();
    }
}

std::shared_ptr<Connection> Context::Get(HCWebsocketHandle hcHandle) const noexcept
{
    auto lock = Lock();
    auto iter = std::find_if(m_connections.begin(), m_connections.end(), [=](auto const& item)
    {
        return item->HcHandle() == hcHandle;
    });

    if (iter != m_connections.end())
    {
        return *iter;
    }
    else
    {
        return nullptr;
    }
}

std::unique_lock<std::mutex> Context::Lock() const noexcept
{
    return std::unique_lock<std::mutex>{ m_mutex };
}

namespace
{

Request Request::Connect(XAsyncBlock* async)
{
    Request r{};
    r.Kind = RequestKind::Connect;
    r.OuterAsync = async;
    return r;
}

Request Request::SendBinary(XAsyncBlock* async, size_t size, uint8_t const* data)
{
    Request r{};
    r.Kind = RequestKind::SendBinary;
    r.OuterAsync = async;
    r.Data.assign(data, data + size);
    return r;
}

Request Request::SendText(XAsyncBlock* async, char const* text)
{
    auto data = reinterpret_cast<uint8_t const*>(text);
    size_t size = strlen(text);

    Request r{};
    r.Kind = RequestKind::SendText;
    r.OuterAsync = async;
    r.Data.assign(data, data + size);
    return r;
}

Request Request::Disconnect()
{
    Request r{};
    r.Kind = RequestKind::Disconnect;
    r.OuterAsync = new XAsyncBlock{}; // we don't get one from libHttpClient
    return r;
}

Connection::Connection(
    Context* WebsocketContext,
    HCWebsocketHandle hc,
    XAsyncBlock* async,
    char const* uri,
    char const* subProtocol
) noexcept :
    m_WebsocketContext{ WebsocketContext },
    m_hcHandle{ hc },
    m_uri{ uri },
    m_subProtocol{ subProtocol },
    m_state{ ConnectionStatus::Created }
{
    HC_TRACE_INFORMATION(HC_WS, "[%p] Websocket: Creating '%s' '%s'", this, m_uri.c_str(), m_subProtocol.c_str());
}

Connection::~Connection() noexcept
{
    m_socket.Finalize();
    HC_TRACE_INFORMATION(HC_WS, "[%p] Websocket: Destroying", this);
}

HCWebsocketHandle Connection::HcHandle() const noexcept
{
    return m_hcHandle; // non mutable state
}

HRESULT Connection::Enqueue(Request&& request) noexcept
{
    auto lock = Lock();
    HC_TRACE_VERBOSE(HC_WS, "[%p:%p] Websocket: Equeue: %s",
        this, request.OuterAsync, StringFromKind(request.Kind));

    try
    {
        m_pending.push_back(std::move(request));
    }
    catch (std::bad_alloc)
    {
        return E_OUTOFMEMORY;
    }

    return RunNextIfPossible();
}

void Connection::Poll() noexcept
{
    // does not access mutable state
    HC_TRACE_VERBOSE(HC_WS, "[%p] Websocket: Polling", this);

    {
        auto lock = Lock();
        StartRequest(std::move(lock));
    }

    if (m_state == ConnectionStatus::Connected || m_state == ConnectionStatus::Closing)
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p] Websocket: Pumping socket", this);
        m_socket.Perform();
    }
}

std::unique_lock<std::mutex> Connection::Lock() const noexcept
{
    return std::unique_lock<std::mutex>{ m_mutex };
}

HRESULT Connection::RunNextIfPossible() noexcept
{
    // assume we are inside the lock
    HC_TRACE_VERBOSE(HC_WS, "[%p] Websocket: Run next if possible", this);

    if (m_running.Kind != RequestKind::None)
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p] Websocket: Busy", this);
        return S_OK;
    }
    else if (m_pending.empty())
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p] Websocket: Nothing to run", this);
        return S_OK;
    }

    std::swap(m_pending.front(), m_running);
    m_pending.pop_front();

    XAsyncBlock* async = m_running.OuterAsync;
    HC_TRACE_VERBOSE(HC_WS, "[%p:%p] Websocket: Starting async: %s", this, async, StringFromKind(m_running.Kind));
    return XAsyncBegin(
        async,
        this,
        reinterpret_cast<void*>(HCWebSocketConnectAsync),
        "Websocket Connection",
        &Connection::WriteProvider
    );
}

void Connection::RegisterInnerAsync(XAsyncBlock* inner) noexcept
{
    // assume we are inside the lock

    assert(!m_running.InnerAsync);
    m_running.InnerAsync = inner;
    HC_TRACE_VERBOSE(HC_WS, "[%p:%p] Websocket: Set inner async to %p",
        this, m_running.OuterAsync, m_running.InnerAsync);
}

void Connection::StartRequest(std::unique_lock<std::mutex>&& lock) noexcept
{
    // assume we are inside the lock

    assert(m_running.InnerAsync || m_running.Kind == RequestKind::None);

    switch (m_running.Kind)
    {
    case RequestKind::None: break; // do nothing
    case RequestKind::Connect:
    {
        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Connect", this, m_running.OuterAsync);
        assert(m_state == ConnectionStatus::Created);

        nn::Result res = m_socket.Initialize();
        if (res.IsFailure())
        {
            HC_TRACE_ERROR(HC_WS, "[%p:%p] Websocket: Failed to init socket: %d-%d: %08X",
                this, m_running.OuterAsync, res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());
            Complete(E_UNEXPECTED, res, true, std::move(lock));
            return;
        }

        m_socket.SetOpenEventHandler(&Connection::OnOpen, this);
        m_socket.SetMessageEventHandler(&Connection::OnMessage, this);
        m_socket.SetCloseEventHandler(&Connection::OnClose, this);
        m_socket.SetErrorEventHandler(&Connection::OnError, this);
        m_socket.SetCancellationFunction(&Connection::OnCancellationCheck, this);

        nn::nifm::SubmitNetworkRequestAndWait();
        if (!nn::nifm::IsNetworkAvailable())
        {
            HC_TRACE_ERROR(HC_WS, "[%p:%p] Websocket: Network unavailable", this, m_running.OuterAsync);
            Complete(E_HC_NO_NETWORK, res, true, std::move(lock));
            return;
        }

        m_socket.SetConnection(&m_connection);

        // Set headers

        // Sub protocol
        if (!m_subProtocol.empty() && false)
        {
            res = m_socket.AddRequestHeader("Sec-WebSocket-Protocol", m_subProtocol.c_str());
            if (res.IsFailure())
            {
                HC_TRACE_ERROR(
                    HC_WS,
                    "[%p:%p] Websocket: Failed to add subprotocol (%s):  %d-%d: %08X",
                    this,
                    m_running.OuterAsync,
                    m_subProtocol.c_str(),
                    res.GetModule(),
                    res.GetDescription(),
                    res.GetInnerValueForDebug()
                );
                Complete(E_UNEXPECTED, res, true, std::move(lock));
                return;
            }
        }

        // client custom headers
        uint32_t headerCount = 0;
        HRESULT hr = HCWebSocketGetNumHeaders(m_hcHandle, &headerCount);
        if (FAILED(hr))
        {
            Complete(hr, nn::Result{}, true, std::move(lock));
            return;
        }

        for (uint32_t i = 0; i < headerCount; ++i)
        {
            char const* key = nullptr;
            char const* value = nullptr;
            hr = HCWebSocketGetHeaderAtIndex(m_hcHandle, i, &key, &value);
            if (FAILED(hr))
            {
                Complete(hr, nn::Result{}, true, std::move(lock));
                return;
            }

            res = m_socket.AddRequestHeader(key, value);
            if (res.IsFailure())
            {
                HC_TRACE_ERROR(
                    HC_WS,
                    "[%p:%p] Websocket: Failed to add header (%s: %s):  %d-%d: %08X",
                    this,
                    m_running.OuterAsync,
                    key,
                    value,
                    res.GetModule(),
                    res.GetDescription(),
                    res.GetInnerValueForDebug()
                );
                Complete(E_UNEXPECTED, res, true, std::move(lock));
                return;
            }
        }

        m_state = ConnectionStatus::Connecting;

        {
            // open can invoke callbacks synchronously
            lock.unlock();
            res = m_socket.Open(m_uri.c_str());
            lock.lock();
        }

        if (res.IsFailure())
        {
            HC_TRACE_ERROR(
                HC_WS,
                "[%p:%p] Websocket: Failed to open url '%s': %d-%d: %08X",
                this,
                m_running.OuterAsync,
                m_uri.c_str(),
                res.GetModule(),
                res.GetDescription(),
                res.GetInnerValueForDebug()
            );
            Complete(E_FAIL, res, true, std::move(lock));
            return;
        }

        // m_socket.Open completes synchronously
        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Connect done", this, m_running.OuterAsync);
    }
    break;
    case RequestKind::SendBinary:
    {
        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Send binary", this, m_running.OuterAsync);
        assert(m_state == ConnectionStatus::Connected);

        nn::Result res = m_socket.SendBinary(m_running.Data.data(), m_running.Data.size());
        if (res.IsFailure())
        {
            HC_TRACE_ERROR(
                HC_WS,
                "[%p:%p] Websocket: Failed to send %zu bytes of binary data: %d-%d: %08X",
                this,
                m_running.OuterAsync,
                m_running.Data.size(),
                res.GetModule(),
                res.GetDescription(),
                res.GetInnerValueForDebug()
            );
            Complete(E_FAIL, res, false, std::move(lock));
            return;
        }

        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Send done", this, m_running.OuterAsync);
        Complete(S_OK, nn::Result{}, false, std::move(lock));
        return;
    }
    break;
    case RequestKind::SendText:
    {
        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Send text", this, m_running.OuterAsync);
        assert(m_state == ConnectionStatus::Connected);

        nn::Result res = m_socket.SendText(
            reinterpret_cast<char const*>(m_running.Data.data()),
            m_running.Data.size()
        );
        if (res.IsFailure())
        {
            HC_TRACE_ERROR(
                HC_WS,
                "[%p:%p] Websocket: Failed to send %zu bytes of text data: %d-%d: %08X",
                this,
                m_running.OuterAsync,
                m_running.Data.size(),
                res.GetModule(),
                res.GetDescription(),
                res.GetInnerValueForDebug()
            );
            Complete(E_FAIL, res, false, std::move(lock));
            return;
        }

        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Send done", this, m_running.OuterAsync);
        Complete(S_OK, nn::Result{}, false, std::move(lock));
        return;
    }
    break;
    case RequestKind::Disconnect:
    {
        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Disconnect", this, m_running.OuterAsync);
        if (m_state == ConnectionStatus::Closing)
        {
            HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Waiting", this, m_running.OuterAsync);
            return;
        }

        assert(m_state == ConnectionStatus::Connected);

        m_state = ConnectionStatus::Closing;
        nn::Result res = m_socket.Close();
        if (res.IsFailure())
        {
            HC_TRACE_ERROR(HC_WS, "[%p:%p] Websocket: Failed to close socket: %d-%d: %08X",
                this, m_running.OuterAsync, res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());
            return;
        }

        // this operation completes asynchronously
        HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Disconnect pending", this, m_running.OuterAsync);
        return;
    }
    break;
    }
}

void Connection::Complete(
    HRESULT hr,
    nn::Result res,
    bool withPayload,
    std::unique_lock<std::mutex>&& lock
) noexcept
{
    // assume we are inside the lock

    HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Completing with result 0x%08X / %d-%d: %08X",
        this, m_running.OuterAsync, hr, res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());

    XAsyncBlock* async = m_running.OuterAsync;
    assert(async);

    // If Connect fails, set connection state to Closed so that things eventually gets cleaned up
    if (m_running.Kind == RequestKind::Connect && FAILED(hr))
    {
        assert(m_state == ConnectionStatus::Created);
        m_state = ConnectionStatus::Closed;
    }

    m_running.Result = res;
    m_completed.emplace_back(); // intentionally die on out of memory here
    std::swap(m_completed.back(), m_running);

    RunNextIfPossible();

    lock.unlock(); // XAsyncComplete may reenter

    XAsyncComplete(async, hr, withPayload ? sizeof(WebSocketCompletionResult) : 0);
}

HRESULT Connection::WriteProvider(XAsyncOp op, XAsyncProviderData const* data) noexcept
{
    Connection* self = static_cast<Connection*>(data->context);

    switch (op)
    {
    case XAsyncOp::Begin:
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p:i%p] Websocket: XAsyncOp::Begin", self, data->async);
        // we are inside a lock here

        if (self->m_state == ConnectionStatus::Closed)
        {
            self->m_completed.emplace_back();
            std::swap(self->m_completed.back(), self->m_running);
            assert(self->m_pending.empty());
            return E_ABORT;
        }
        self->RegisterInnerAsync(data->async);
    }
    break;
    case XAsyncOp::DoWork:
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p:i%p] Websocket: XAsyncOp::DoWork", self, data->async);
        assert(false);
    }
    break;
    case XAsyncOp::GetResult:
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p:i%p] Websocket: XAsyncOp::GetResult", self, data->async);
        auto lock = self->Lock();

        for (auto& r : self->m_completed)
        {
            if (r.InnerAsync != data->async)
            {
                continue;
            }

            HC_TRACE_VERBOSE(HC_WS, "[%p:%p] Websocket: Found request", self, r.OuterAsync);
            auto result = reinterpret_cast<WebSocketCompletionResult*>(data->buffer);
            result->websocket = self->m_hcHandle;
            result->platformErrorCode = r.Result.GetInnerValueForDebug();
            result->errorCode = XAsyncGetStatus(data->async, false);

            return S_OK;
        }

        HC_TRACE_ERROR(HC_WS, "[%p:i%p] Websocket: No request matching the async block found", self, data->async);
        assert(false);

        auto result = reinterpret_cast<WebSocketCompletionResult*>(data->buffer);
        result->websocket = self->m_hcHandle;
        result->platformErrorCode = nn::Result{}.GetInnerValueForDebug();
        result->errorCode = E_UNEXPECTED;
    }
    break;
    case XAsyncOp::Cancel: break;
    case XAsyncOp::Cleanup:
    {
        HC_TRACE_VERBOSE(HC_WS, "[%p:i%p] Websocket: XAsyncOp::Cleanup", self, data->async);
        auto lock = self->Lock();

        self->m_completed.erase(std::remove_if(
            self->m_completed.begin(),
            self->m_completed.end(),
            [&](auto it)
            {
                if (it.InnerAsync == data->async)
                {
                    HC_TRACE_VERBOSE(HC_WS, "[%p:i%p] Websocket: Cleaning up request", self, data->async);
                    return true;
                }
                return false;
            }
        ), self->m_completed.end());

        // If we are in a closed state with no completed operations remaining, it is safe to clean up
        if (self->m_state == ConnectionStatus::Closed && self->m_completed.empty())
        {
            HC_TRACE_VERBOSE(HC_WS, "[%p:i%p] Websocket: Disconnect completed, destroying connection",
                self, data->async);

            assert(self->m_pending.empty());
            lock.unlock();
            self->m_WebsocketContext->Unregister(*self);
            // self is destroyed from here on
        }

        return S_OK;
    }
    break;
    }

    return S_OK;
}

void Connection::OnOpen(void* ctx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    assert(ctx);
    auto self = static_cast<Connection*>(ctx);

    auto lock = self->Lock();
    HC_TRACE_INFORMATION(HC_WS, "Websocket: Opened");

    assert(self->m_state == ConnectionStatus::Connecting);
    assert(self->m_running.Kind == RequestKind::Connect);
    assert(self->m_socket.IsAvailable());

    self->m_state = ConnectionStatus::Connected;
    self->Complete(S_OK, nn::Result{}, true, std::move(lock));
}

void Connection::OnMessage(void* data, size_t size, bool isText, void* ctx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    assert(ctx);
    auto self = static_cast<Connection*>(ctx);

    HC_TRACE_INFORMATION(HC_WS, "Websocket: Received message");

    HCWebSocketMessageFunction textCallback = nullptr;
    HCWebSocketBinaryMessageFunction binaryCallback = nullptr;
    void* callbackWebsocketContext = nullptr;
    HRESULT hr = HCWebSocketGetEventFunctions(
        self->m_hcHandle, // const, safe to read outside lock
        &textCallback,
        &binaryCallback,
        nullptr,
        &callbackWebsocketContext
    );
    assert(SUCCEEDED(hr));

    if (isText)
    {
        if (textCallback)
        {
            // copy into a string to get a null terminated buffer
            std::string text{ static_cast<char*>(data), static_cast<char*>(data) + size };
            textCallback(self->m_hcHandle, text.c_str(), callbackWebsocketContext);
        }
    }
    else
    {
        if (binaryCallback)
        {
            binaryCallback(
                self->m_hcHandle,
                static_cast<uint8_t*>(data),
                static_cast<uint32_t>(size),
                callbackWebsocketContext
            );
        }
    }
}

void Connection::OnClose(uint16_t code, char const* reason, size_t size, void* ctx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    assert(ctx);
    auto self = static_cast<Connection*>(ctx);

    // If the running operation was a Disconnect, mark it as complete
    std::unique_ptr<XAsyncBlock> disconnectAsync;
    {
        auto lock = self->Lock();
        HC_TRACE_INFORMATION(HC_WS, "Websocket: Closed %u - '%.*s'", code, size, reason);

        self->m_state = ConnectionStatus::Closed;

        // If this was an expected close, complete the disconnect request
        if (self->m_running.Kind == RequestKind::Disconnect)
        {
            HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Shutting down", self, self->m_running.OuterAsync);
            disconnectAsync.reset(self->m_running.OuterAsync);

            self->m_completed.emplace_back(); // intentionally die on out of memory here
            std::swap(self->m_completed.back(), self->m_running);
        }

        // Also flush any pending requests with a failure
        for (auto& r : self->m_pending)
        {
            XAsyncBegin(r.OuterAsync, self, nullptr, __FUNCTION__,
                [](XAsyncOp op, const XAsyncProviderData* data)
            {
                Connection* self = static_cast<Connection*>(data->context);
                switch (op)
                {
                case XAsyncOp::Begin:
                {
                    HC_TRACE_INFORMATION(HC_WS, "[%p:%p] Websocket: Complete pending operation", self, data->async);
                    return E_ABORT;
                }
                default:
                {
                    return S_OK;
                }
                }
            });
        }

        self->m_pending.clear();

        if (self->m_completed.empty())
        {
            HC_TRACE_INFORMATION(HC_WS, "[%p] Websocket: Unregistering disconnected websocket", self);
            self->m_WebsocketContext->Unregister(*self);
        }
    }

    if (disconnectAsync)
    {
        XAsyncComplete(disconnectAsync.get(), S_OK, false);
    }
    

    HCWebsocketHandle hcHandle = self->m_hcHandle; // const, safe to read outside lock
    HCWebSocketCloseEventFunction callback = nullptr;
    void* callbackWebsocketContext = nullptr;
    HRESULT hr = HCWebSocketGetEventFunctions(
        hcHandle,
        nullptr,
        nullptr,
        &callback,
        &callbackWebsocketContext
    );
    assert(SUCCEEDED(hr));

    if (callback)
    {
        callback(hcHandle, static_cast<HCWebSocketCloseStatus>(code), callbackWebsocketContext);
    }
}

void Connection::OnError(nn::Result res, void* ctx) noexcept
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    assert(ctx);
    auto self = static_cast<Connection*>(ctx);

    auto lock = self->Lock();

    HC_TRACE_ERROR(HC_WS, "Websocket error: %d-%d: %08X",
        res.GetModule(), res.GetDescription(), res.GetInnerValueForDebug());

    switch (self->m_running.Kind)
    {
    case RequestKind::Connect:
    {
        // connect will get this result directly
        break;
    }
    case RequestKind::SendBinary: // intentional fallthrough
    case RequestKind::SendText:
    {
        if (nn::websocket::ResultSendError::Includes(res))
        {
            auto async{ self->m_running.OuterAsync };
            assert(async);

            self->m_running.Result = res;
            self->m_completed.emplace_back(); // intentionally die on out of memory here
            std::swap(self->m_completed.back(), self->m_running);

            lock.unlock(); // XAsyncComplete may reenter

            XAsyncComplete(async, E_FAIL, sizeof(WebSocketCompletionResult));
        }
        break;
    }
    case RequestKind::Disconnect:
    {
        // if we were disconnecting we will get this result via OnClose
        break;
    }
    case RequestKind::None:
    {
        // failure could be detected during a receive or heartbeat. OnClose will be invoked with failure code later
        break;
    }
    }
}

bool Connection::OnCancellationCheck(void* /*ctx*/) noexcept
{
    return false;
}

HRESULT CALLBACK ConnectAsync(
    _In_z_ const char* uri,
    _In_z_ const char* subProtocol,
    _In_ HCWebsocketHandle websocket,
    _Inout_ XAsyncBlock* asyncBlock,
    _In_opt_ void* context,
    _In_ HCPerformEnv /*env*/
)
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    auto& ctx = *static_cast<Context*>(context);
    auto conn = std::make_unique<Connection>(&ctx, websocket, asyncBlock, uri, subProtocol);

    HRESULT hr = conn->Enqueue(Request::Connect(asyncBlock));
    if (SUCCEEDED(hr))
    {
        ctx.Register(std::move(conn));
    }

    return hr;
}

HRESULT CALLBACK SendMessageAsync(
    _In_ HCWebsocketHandle websocket,
    _In_z_ const char* message,
    _Inout_ XAsyncBlock* asyncBlock,
    _In_opt_ void* context
)
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    auto& ctx = *static_cast<Context*>(context);
    auto conn = ctx.Get(websocket);
    if (conn)
    {
        auto r = Request::SendText(asyncBlock, message);
        return conn->Enqueue(std::move(r));
    }
    else
    {
        assert(false);
        return E_INVALIDARG;
    }
}

HRESULT CALLBACK SendBinaryMessageAsync(
    _In_ HCWebsocketHandle websocket,
    _In_reads_bytes_(payloadSize) const uint8_t* payloadBytes,
    _In_ uint32_t payloadSize,
    _Inout_ XAsyncBlock* asyncBlock,
    _In_opt_ void* context
)
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    auto& ctx = *static_cast<Context*>(context);
    auto conn = ctx.Get(websocket);
    if (conn)
    {
        auto r = Request::SendBinary(asyncBlock, payloadSize, payloadBytes);
        conn->Enqueue(std::move(r));
    }
    else
    {
        assert(false);
        return E_INVALIDARG;
    }

    return S_OK;
}

HRESULT CALLBACK Disconnect(
    _In_ HCWebsocketHandle websocket,
    _In_ HCWebSocketCloseStatus closeStatus,
    _In_opt_ void* context
)
{
    HC_TRACE_SCOPE_VERBOSE(HC_WS);
    auto& ctx = *static_cast<Context*>(context);
    auto conn = ctx.Get(websocket);
    if (conn)
    {
        auto r = Request::Disconnect();
        conn->Enqueue(std::move(r));
    }
    else
    {
        assert(false);
        return E_INVALIDARG;
    }

    return S_OK;
}

}

}

HRESULT InitWebSocketHooks(WebSocket::Context** contextPtr)
{
    assert(contextPtr);

    auto context = std::make_unique<WebSocket::Context>();

    HRESULT hr = HCSetWebSocketFunctions(
        &WebSocket::ConnectAsync,
        &WebSocket::SendMessageAsync,
        &WebSocket::SendBinaryMessageAsync,
        &WebSocket::Disconnect,
        context.get()
    );
    if (FAILED(hr)) { return hr; }

    *contextPtr = context.release();

    return S_OK;
}

void UpdateWebSocketHooks(WebSocket::Context* contextPtr)
{
    assert(contextPtr);
    contextPtr->PollAll();
}

void CleanupWebSocketHooks(WebSocket::Context* contextPtr)
{
    assert(contextPtr);
    std::unique_ptr<WebSocket::Context>{ contextPtr };
}
