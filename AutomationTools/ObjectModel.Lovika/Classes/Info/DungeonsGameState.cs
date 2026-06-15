// ------------------------------------------------------------------------------
// <copyright file="DungeonsGameState.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Info
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

    [GameClass("DungeonsGameState")]
    [GameProperty("IsGameOver", "mCinematic.mLevelSequencePtr.SequencePlayer.Status")]
    public class DungeonsGameState : AActor
    {
        public DungeonsGameState(IGameObject obj) : base(obj)
        {
        }

        public bool IsGameOver => NumericHelpers.ParseBool(this["IsGameOver"] as string) ?? false;

        public string CinematicStatus => this["mCinematic.mLevelSequencePtr.SequencePlayer.Status"] as string;

        public bool IsCinematicPlaying() => this.CinematicStatus == "Playing";
    }
}
