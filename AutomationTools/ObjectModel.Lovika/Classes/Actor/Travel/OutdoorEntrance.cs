// ------------------------------------------------------------------------------
// <copyright file="OutdoorEntrance.cs" company="Microsoft">
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

    [GameClass("BP_FF_OutdoorEntrance_New_C")]
    [GameProperty("Blocked")]
    public class OutdoorEntrance : InstantTravel
    {
        public OutdoorEntrance(IGameObject obj) : base(obj)
        {
        }

        public bool Blocked => NumericHelpers.ParseBool(this["Blocked"] as string) ?? false;
    }
}
