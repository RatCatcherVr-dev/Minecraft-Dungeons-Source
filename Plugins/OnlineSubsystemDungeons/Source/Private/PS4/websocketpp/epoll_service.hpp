#pragma once

#include <atomic>
#include <unordered_map>

#include <websocketpp/common/thread.hpp>

#include <net.h>

namespace websocketpp {
namespace orbis {

class epoll_service {
public:
	enum event_type {
		pollin = 0,
		pollout = 1,
		max_types = 2
	};
	typedef lib::function<void()> callback;

	epoll_service() {
		m_epoll = sceNetEpollCreate("websocket", 0);
	}

	~epoll_service() {
		_cleanup();
		sceNetEpollDestroy(m_epoll);
	}

	void work(int timeoutMilliseconds) {
		_work(timeoutMilliseconds);
	}

	int registerDescriptor(SceNetId descriptor) {
		std::shared_ptr<epoll_data> epollData = std::make_shared<epoll_data>();

		SceNetEpollEvent ev;
		ev.events = SCE_NET_EPOLLIN | SCE_NET_EPOLLOUT;
		ev.data.ptr = epollData.get();

		int sceResult = sceNetEpollControl(m_epoll, SCE_NET_EPOLL_CTL_ADD, descriptor, &ev);
		if (sceResult == 0) {
			std::lock_guard<decltype(m_epoll_data_mutex)> lock(m_epoll_data_mutex);
			m_epoll_data_map[descriptor] = std::move(epollData);
		}
		return sceResult;
	}

	int deregisterDescriptor(SceNetId descriptor) {
		int sceResult = sceNetEpollControl(m_epoll, SCE_NET_EPOLL_CTL_DEL, descriptor, nullptr);

		{
			std::lock_guard<decltype(m_epoll_data_mutex)> lock(m_epoll_data_mutex);
			m_epoll_data_map.erase(descriptor);
		}

		return sceResult;
	}

	bool addCallback(event_type type, SceNetId descriptor, callback cb) {
		if (cb == nullptr) {
			return false;
		}

		std::shared_ptr<epoll_data> epollData;
		{
			std::lock_guard<decltype(m_epoll_data_mutex)> lock(m_epoll_data_mutex);
			auto it = m_epoll_data_map.find(descriptor);
			if (it != m_epoll_data_map.end()) {
				epollData = it->second;
			}
		}

		const bool epollDataExists = epollData != nullptr;
		if (epollDataExists) {
			std::lock_guard<decltype(epollData->m_mutex)> lock(epollData->m_mutex);
			epollData->m_callbacks[type].push_back(cb);
		}

		return epollDataExists;
	}


private:
	void _work(int timeoutMilliseconds) {
		constexpr int maxEvents = 128;
		SceNetEpollEvent events[maxEvents];
		std::vector<callback> callbacks;

		int sceResult = sceNetEpollWait(m_epoll, events, maxEvents, timeoutMilliseconds);
		for (int i = 0; i < sceResult; ++i) {
			const auto& ev = events[i];
			auto epollData = static_cast<epoll_data*>(ev.data.ptr);
			static const int flags[event_type::max_types] = { SCE_NET_EPOLLIN, SCE_NET_EPOLLOUT };
			for (int j = event_type::max_types - 1; j >= 0; --j) {
				if ((ev.events & (flags[j] | SCE_NET_EPOLLHUP | SCE_NET_EPOLLERR)) != 0) {
					std::vector<callback> tempCallbacks;
					{
						std::lock_guard<decltype(epollData->m_mutex)> lock(epollData->m_mutex);
						std::swap(tempCallbacks, epollData->m_callbacks[j]);
					}

					callbacks.insert(callbacks.end(),
						std::make_move_iterator(tempCallbacks.begin()),
						std::make_move_iterator(tempCallbacks.end()));
				}
			}
		}

		for (auto&& cb : callbacks) {
			cb();
		}
	}

	void _cleanup() {
		std::lock_guard<decltype(m_epoll_data_mutex)> lock(m_epoll_data_mutex);
		m_epoll_data_map.clear();
	}

	struct epoll_data {
		std::vector<callback> m_callbacks[event_type::max_types];
		std::mutex m_mutex;
	};

	SceNetId m_epoll{ -1 };

	std::unordered_map<SceNetId, std::shared_ptr<epoll_data>> m_epoll_data_map;
	std::mutex m_epoll_data_mutex;
};

}
}
