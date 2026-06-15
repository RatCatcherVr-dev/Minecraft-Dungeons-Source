#include "ssl_context.h"

namespace Ssl
{

void Deleter::operator()(nn::ssl::Context* ctx) noexcept
{
    ctx->Destroy();
}

Context Make() noexcept
{
    Context ssl{ new nn::ssl::Context() };

    nn::Result res = ssl->Create(nn::ssl::Context::SslVersion_Auto);
    if (res.IsFailure())
    {
        assert(false);
    }

    return ssl;
}

}
