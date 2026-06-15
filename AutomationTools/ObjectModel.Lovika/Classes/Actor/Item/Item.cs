// ------------------------------------------------------------------------------
// <copyright file="Item.cs" company="Microsoft">
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
    using Microsoft.GNS.GameStateTracker.Infrastructure.TreeObject;

    [GameClass("Item")]
    [GameProperty("ItemId")]
    public class Item : PropActor
    {
        public Item(IGameObject obj) : base(obj)
        {
        }

        public string ItemId => this.ItemIdFromSeralized(this["ItemId"] as string);

        private string ItemIdFromSeralized(string input)
        {
            var rootTree = Parser.TryParse(input) as Node;
            if (rootTree == null)
            {
                return null;
            }

            return (rootTree.First(kv => kv.Key == "SerializedId").Value as Leaf)?.Value;
        }
    }
}
