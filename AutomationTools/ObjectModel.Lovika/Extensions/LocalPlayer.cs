// ------------------------------------------------------------------------------
// <copyright file="LocalPlayer.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Player;

    /// <summary>
    /// Extension methods to retrieve the local player controller and pawn from <see cref="IGameState" />.
    /// </summary>
    public static partial class IGameStateExtensions
    {
        /// <summary>
        /// Gets the cached local player controller with the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <returns>A player controller.</returns>
        public static PlayerController PlayerController(this IGameState self, int playerIndex)
        {
            return self.ExistingAll<PlayerController>().Where(i => i.NetPlayerIndex == playerIndex).FirstOrDefault();
        }

        /// <summary>
        /// Gets the cached local player controller with the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <returns>A player controller.</returns>
        public static PlayerMenuController PlayerMenuController(this IGameState self, int playerIndex)
        {
            return self.ExistingAll<PlayerMenuController>().Where(i => i.NetPlayerIndex == playerIndex).FirstOrDefault();
        }

        /// <summary>
        /// Gets the cached local player pawn with the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <returns>A player pawn.</returns>
        public static PlayerPawn PlayerCharacter(this IGameState self, int playerIndex)
        {
            return self.PlayerController(playerIndex).Pawn();
        }

        /// <summary>
        /// Gets the local player controller with the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <param name="maxAge">The maximum age of the object, if it exists in the cache.</param>
        /// <returns>A player controller.</returns>
        public static async Task<PlayerController> GetPlayerController(this IGameState self, int playerIndex, TimeSpan? maxAge = null)
        {
            return (await self.GetAll<PlayerController>(maxAge).ConfigureAwait(false)).Where(i => i.NetPlayerIndex == playerIndex).FirstOrDefault();
        }

        /// <summary>
        /// Gets the local player controller with the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <param name="maxAge">The maximum age of the object, if it exists in the cache.</param>
        /// <returns>A player controller.</returns>
        public static async Task<PlayerMenuController> GetPlayerMenuController(this IGameState self, int playerIndex, TimeSpan? maxAge = null)
        {
            return (await self.GetAll<PlayerMenuController>(maxAge).ConfigureAwait(false)).Where(i => i.NetPlayerIndex == playerIndex).FirstOrDefault();
        }

        /// <summary>
        /// Gets the local player pawn with the given player index.
        /// </summary>
        /// <param name="self">The game state.</param>
        /// <param name="playerIndex">The player index.</param>
        /// <param name="maxAge">The maximum age of the object, if it exists in the cache.</param>
        /// <returns>A player pawn.</returns>
        public static async Task<PlayerPawn> GetPlayerCharacter(this IGameState self, int playerIndex, TimeSpan? maxAge = null)
        {
            PlayerControllerBase player = await self.GetPlayerController(playerIndex, maxAge).ConfigureAwait(false);
            if (player == null)
            {
                return null;
            }

            return await player.GetPawn(TimeSpan.Zero).ConfigureAwait(false);
        }
    }
}