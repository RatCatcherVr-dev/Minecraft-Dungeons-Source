// ------------------------------------------------------------------------------
// <copyright file="PlayerControllerBase.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Player
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.GameToolkit;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;

    /// <summary>
    /// A class representing a player controller.
    /// </summary>
    [GameClass("PlayerControllerBase")]
    [GameProperty("NetPlayerIndex", "bGamepadActive", "bPlayerIsWaiting")]
    public class PlayerControllerBase : AController
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PlayerControllerBase" /> class.
        /// </summary>
        /// <param name="obj">The game object to encapsulate.</param>
        public PlayerControllerBase(IGameObject obj) : base(obj)
        {
        }

        /// <summary>
        /// Gets the player's net index.
        /// </summary>
        public int NetPlayerIndex => NumericHelpers.ParseInt(this["NetPlayerIndex"] as string) ?? 0;

        public bool GamepadActive => NumericHelpers.ParseBool(this["bGamepadActive"] as string) ?? false;

        public bool PlayerIsWaiting => NumericHelpers.ParseBool(this["bPlayerIsWaiting"] as string) ?? true;

        /// <summary>
        /// Gets the cached controlled pawn.
        /// </summary>
        /// <returns>A pawn object.</returns>
        public PlayerPawn Pawn()
        {
            return this.GameState.Existing<PlayerPawn>(this.PawnInfo);
        }

        /// <summary>
        /// Gets the controlled pawn.
        /// </summary>
        /// <param name="maxAge">The maximum age of the object, if it exists in the cache.</param>
        /// <returns>A pawn object.</returns>
        public Task<PlayerPawn> GetPawn(TimeSpan? maxAge = null)
        {
            return this.GameState.Get<PlayerPawn>(this.PawnInfo, maxAge);
        }

        /// <summary>
        /// Gets cached the player state.
        /// </summary>
        /// <returns>A player state object.</returns>
        public PlayerInfo PlayerState()
        {
            return this.GameState.Existing<PlayerInfo>(this.PlayerStateInfo);
        }

        /// <summary>
        /// Gets the player state.
        /// </summary>
        /// <param name="maxAge">The maximum age of the object, if it exists in the cache.</param>
        /// <returns>A player state object.</returns>
        public Task<PlayerInfo> GetPlayerState(TimeSpan? maxAge = null)
        {
            return this.GameState.Get<PlayerInfo>(this.PlayerStateInfo, maxAge);
        }
    }
}
