// ------------------------------------------------------------------------------
// <copyright file="GameBP.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Info
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Linq;
    using System.Numerics;
    using System.Reflection;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.GameToolkit;

    [GameClass("GameBP")]
    [GameProperty("mHasGameplayStarted", "bUsesObjectives", "mObjectiveLocations")]
    public class GameBP : AGameInfo
    {
        public GameBP(IGameObject obj) : base(obj)
        {
        }

        // This is a private field and so will always be false
        public bool HasGameplayStarted => NumericHelpers.ParseBool(this["mHasGameplayStarted"] as string) ?? false;

        public bool UsesObjectives => NumericHelpers.ParseBool(this["bUsesObjectives"] as string) ?? false;

        // Clickable locations only!
        public ObjectiveLocations ObjectiveLocations => ObjectiveLocations.TryParse(this["mObjectiveLocations"] as string) ?? default;
    }
}
