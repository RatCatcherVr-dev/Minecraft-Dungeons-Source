// ------------------------------------------------------------------------------
// <copyright file="GuideBeacon.cs" company="Microsoft">
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

    [GameClass("BP_GuideBeacon_C")]
    [GameProperty("ReplicatedInteractable.bInteractionEnabled")]
    public class GuideBeacon : PropActor
    {
        public GuideBeacon(IGameObject obj) : base(obj)
        {
        }

        public bool InteractionEnabled => NumericHelpers.ParseBool(this["ReplicatedInteractable.bInteractionEnabled"] as string) ?? true;
    }
}
