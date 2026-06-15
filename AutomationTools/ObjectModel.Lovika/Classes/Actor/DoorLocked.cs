// ------------------------------------------------------------------------------
// <copyright file="DoorLocked.cs" company="Microsoft">
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

    [GameProperty("IsOpen", "ReplicatedInteractable.bInteractionEnabled")]
    public class DoorLocked : PropActor
    {
        public DoorLocked(IGameObject obj) : base(obj)
        {
        }

        public bool IsOpen => NumericHelpers.ParseBool(this["IsOpen"] as string) ?? false;

        public bool InteractionEnabled => NumericHelpers.ParseBool(this["ReplicatedInteractable.bInteractionEnabled"] as string) ?? true;
    }
}
