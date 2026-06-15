//------------------------------------------------------------------------------
// <copyright file="RpcAnnotations.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#ifdef ENABLE_GAMESTEST_RPC

#define GAMESTEST_ANNOTATE_HRESULT_METHOD _Check_return_ _Success_(!FAILED(return))

#endif