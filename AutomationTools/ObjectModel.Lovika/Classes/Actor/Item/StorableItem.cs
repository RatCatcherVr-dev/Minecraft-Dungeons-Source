// ------------------------------------------------------------------------------
// <copyright file="StorableItem.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor.Item
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameToolkit;

    [GameClass("StorableItem")]
    [GameProperty("lockItemToOwner", "ItemData")]
    public class StorableItem : Item
    {
        public StorableItem(IGameObject obj) : base(obj)
        {
        }

        public bool LockItemToOwner => NumericHelpers.ParseBool(this["lockItemToOwner"] as string) ?? false;

        public object ItemData => this["ItemData"];
    }
}
