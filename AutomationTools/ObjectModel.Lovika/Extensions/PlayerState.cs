// ------------------------------------------------------------------------------
// <copyright file="PlayerState.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Player;

    /// <summary>
    /// Extension methods to retrieve player states from <see cref="IGameState" />.
    /// </summary>
    public static partial class IGameStateExtensions
    {
        /// <summary>
        /// Gets the cached local player info of the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <returns>A player info object.</returns>
        public static PlayerInfo LocalPlayerState(this IGameState self, int playerIndex)
        {
            return self.PlayerController(playerIndex)?.PlayerState();
        }

        /// <summary>
        /// Gets the local player info of the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <param name="maxAge">The maximum age of the object, if it exists in the cache.</param>
        /// <returns>A player info object.</returns>
        public static async Task<PlayerInfo> GetLocalPlayerState(this IGameState self, int playerIndex, TimeSpan? maxAge = null)
        {
            PlayerControllerBase player = await self.GetPlayerController(playerIndex, maxAge).ConfigureAwait(false);
            if (player == null)
            {
                return null;
            }

            return await player.GetPlayerState(maxAge).ConfigureAwait(false);
        }
    }
}