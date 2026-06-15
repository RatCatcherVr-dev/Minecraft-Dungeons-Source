// ------------------------------------------------------------------------------
// <copyright file="TravelDoorBase.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor.Travel
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameToolkit;

    [GameClass("BP_TravelDoor_Base_C")]
    [GameProperty("Blocked")]
    public class TravelDoorBase : InstantTravel
    {
        public TravelDoorBase(IGameObject obj) : base(obj)
        {
        }

        public bool Blocked => NumericHelpers.ParseBool(this["Blocked"] as string) ?? false;
    }
}
