// ------------------------------------------------------------------------------
// <copyright file="InstantTravel.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor.Travel
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

    [GameClass("InstantTravelActor")]
    [GameProperty("Blocked")]
    public class InstantTravel : AActor
    {
        public InstantTravel(IGameObject obj) : base(obj)
        {
        }

        public bool Blocked => NumericHelpers.ParseBool(this["Blocked"] as string) ?? false;
    }
}
