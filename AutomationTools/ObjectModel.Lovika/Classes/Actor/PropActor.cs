// ------------------------------------------------------------------------------
// <copyright file="PropActor.cs" company="Microsoft">
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
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.GameToolkit;

    [GameClass("PropActor")]
    [GameProperty("rotation", "mCheckDistance", "mPlayerVisible", "mDoPlayerDistanceCheck", "mCanMove")]
    public class PropActor : AActor
    {
        public PropActor(IGameObject obj) : base(obj)
        {
        }

        public float CheckDistance => NumericHelpers.ParseFloat(this["mCheckDistance"] as string) ?? 3500.0f; // standard (3500.0f) is equivalent to 35 tiles

        public bool PlayerVisible => NumericHelpers.ParseBool(this["mPlayerVisible"] as string) ?? true;

        public bool DoPlayerDistanceCheck => NumericHelpers.ParseBool(this["mDoPlayerDistanceCheck"] as string) ?? true;

        public bool CanMove => NumericHelpers.ParseBool(this["mCanMove"] as string) ?? false;
    }
}
