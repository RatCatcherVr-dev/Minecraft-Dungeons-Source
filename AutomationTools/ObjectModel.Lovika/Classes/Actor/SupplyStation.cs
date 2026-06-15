// ------------------------------------------------------------------------------
// <copyright file="SupplyStation.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameToolkit;

    [GameClass("BP_SupplyStation_C")]
    [GameProperty("OpenCount")]
    public class SupplyStation : PropActor
    {
        public SupplyStation(IGameObject obj) : base(obj)
        {
        }

        public int OpenCount => NumericHelpers.ParseInt(this["OpenCount"] as string) ?? 0;
    }
}
