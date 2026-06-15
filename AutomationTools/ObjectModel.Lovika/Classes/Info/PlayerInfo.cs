// ------------------------------------------------------------------------------
// <copyright file="PlayerInfo.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Info
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.GameToolkit;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Player;

    /// <summary>
    /// A class representing player info.
    /// </summary>
    [GameClass("BasePlayerState")]
    [GameProperty("PlayerNumber")]
    public class PlayerInfo : APlayerInfo
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PlayerInfo" /> class.
        /// </summary>
        /// <param name="obj">The game object to encapsulate.</param>
        public PlayerInfo(IGameObject obj) : base(obj)
        {
        }

        public int PlayerNumber => NumericHelpers.ParseInt(this["PlayerNumber"] as string) ?? 0;

        /// <summary>
        /// Gets the player's controlled pawn.
        /// </summary>
        /// <returns>A pawn object.</returns>
        public PlayerPawn Pawn()
        {
            return this.GameState.ExistingAll<PlayerPawn>().FirstOrDefault(p => p["PlayerState"] as string == this.Info.ToString());
        }
    }
}
