//------------------------------------------------------------------------------
// <copyright file="RpcMemory.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC

#include <memory>

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server
{
	class RpcArchiveImpl;

	// Smart pointer for archive data that helps with std::move semantics and supports pool allocation.
	typedef void (*RpcArchiveImplDeleter)(RpcArchiveImpl*);
	typedef std::unique_ptr<RpcArchiveImpl, RpcArchiveImplDeleter> RpcArchiveImplPtr;

}}}}};

#endif