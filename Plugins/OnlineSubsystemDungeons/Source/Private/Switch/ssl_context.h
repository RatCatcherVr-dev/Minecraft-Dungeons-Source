#pragma once

#include <memory>
#include <nn/ssl.h>

namespace Ssl
{

struct Deleter
{
    void operator()(nn::ssl::Context* ctx) noexcept;
};

using Context = std::unique_ptr<nn::ssl::Context, Deleter>;

Context Make() noexcept;

}
