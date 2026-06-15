//------------------------------------------------------------------------------
// <copyright file="CommonRpcActor.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "CommonRpcActor.h"
#include "RpcArchive.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

UTF_ANNOTATE_HRESULT_METHOD HRESULT GetActorLocation(const AActor* Actor, RpcArchive& Output)
{
    if (Actor)
    {
        HRESULT Result = S_OK;
	
        FRpcVector Location(Actor->GetActorLocation());
        HRCHK(Output.SetNamedValue(TEXT("Location"), Location))
        return Location.Result;
    }

    RETURN_ELEMNOTFND("Actor")
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT SetActorLocation(AActor* Actor, RpcArchive& Input, RpcArchive& Output)
{
    if (Actor)
    {
        HRESULT Result = S_OK;

        RpcArchive LocationArchive;
        HRCHK(Input.GetNamedValue(TEXT("Location"), &LocationArchive))
        FRpcVector Location(LocationArchive);
        HRCHK(Location.Result)

        bool bSuccess = Actor->SetActorLocation(Location);
        if (!bSuccess)
        {
            RETURN_FUNCFAILED("AActor::SetActorLocation")
        }

        return Result;
    }

    RETURN_ELEMNOTFND("Actor")
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT GetActorCanBeDamaged(const AActor* Actor, RpcArchive& Output)
{
    if (Actor)
    {
        HRESULT Result = S_OK;
        HRCHK(Output.SetNamedValue(TEXT("CanBeDamaged"), Actor->bCanBeDamaged))
        return Result;
    }

    RETURN_ELEMNOTFND("Actor")
}


UTF_ANNOTATE_HRESULT_METHOD HRESULT SetActorCanBeDamaged(AActor* Actor, RpcArchive& Input, RpcArchive& Output)
{
    if (Actor)
    {
        HRESULT Result = S_OK;

        bool bCanBeDamaged;
        HRCHK(Input.GetNamedValue(TEXT("CanBeDamaged"), &bCanBeDamaged))
        Actor->bCanBeDamaged = bCanBeDamaged;

        return Result;
    }

    RETURN_ELEMNOTFND("Actor")
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
