//------------------------------------------------------------------------------
// <copyright file="RpcLocalPlayer.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "UTFAnnotations.h"

#ifdef ENABLE_GAMESTEST_RPC

#include "RpcPawn.h"
#include "RpcRotator.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace Rpc { namespace Server {
    class RpcArchive;
}}}}}

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

    /**
     * Player data representation.
     */
    class UNREALTOOLSFRAMEWORK_API FRpcLocalPlayer
    {
    public:
	    /** Pawn's data. */
	    FRpcPawn PawnData;

        /** Index of the player. */
        int32 PlayerIndex;

	    /** Player's rotation. */
	    FRpcRotator Rotation;

        /**
         * Id of the controller this player is
         * accepting input from.
         */
        int32 ControllerId;

        /**
         * True if this player is not using splitscreen, or if
         * it is the first player in the splitscreen layout.
         */
        bool bIsPrimaryPlayer;

        /** Is this player playing splitscreen? */
        bool bIsSplitscreenPlayer;

        /**
         * Stores the result of serializing or
         * de-serializing an RpcArchive.
         */
        HRESULT Result;

    public:
        /**
         * Initializes data to default values.
         */
        FRpcLocalPlayer();

        /**
         * Initializes this object with data from the provided player.
         * @param Player - Player to use for initialization.
         * @param Index - Index of the player being created.
         *                Coming from the game instance player index.
         */
        FRpcLocalPlayer(const ULocalPlayer* Player, int32 Index);

        /**
         * Returns an RpcArchive created with this instance's data.
         */
        operator Rpc::Server::RpcArchive();
    };

}}}}

#endif // ENABLE_GAMESTEST_RPC
