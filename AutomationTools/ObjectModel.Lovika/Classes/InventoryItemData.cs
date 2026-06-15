// ------------------------------------------------------------------------------
// <copyright file="InventoryItemData.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure.TreeObject;
    using Microsoft.GNS.GameToolkit;

    public struct InventoryItemData
    {
        public string ItemType { get; private set; }

        public float ItemPower { get; private set; }

        public IEnumerable<string> Enchantments { get; private set; }

        public IEnumerable<string> ArmorProperties { get; private set; }

        public string Rarity { get; private set; }

        public bool IsUpgraded { get; private set; }

        public static InventoryItemData? TryParse(string input)
        {
            var rootTree = Parser.TryParse(input) as Node;
            if (rootTree == null)
            {
                return null;
            }

            var itemData = default(InventoryItemData);

            foreach (var node in rootTree)
            {
                if (node.Key == "ItemType" && node.Value is Leaf)
                {
                    itemData.ItemType = node.Value.Value;
                }
                else if (node.Key == "ItemPower" && node.Value is Leaf)
                {
                    itemData.ItemPower = NumericHelpers.ParseFloat(node.Value.Value) ?? 0;
                }
                else if (node.Key == "Enchantments" && node.Value is Node)
                {
                    var list = new List<string>();

                    foreach (var enchNode in node.Value as Node)
                    {
                        // TODO
                    }

                    itemData.Enchantments = list;
                }
                else if (node.Key == "ArmorProperties" && node.Value is Node)
                {
                    var list = new List<string>();

                    foreach (var armNode in node.Value as Node)
                    {
                        // TODO
                    }

                    itemData.ArmorProperties = list;
                }
                else if (node.Key == "Rarity" && node.Value is Leaf)
                {
                    itemData.Rarity = node.Value.Value;
                }
                else if (node.Key == "bIsUpgraded" && node.Value is Leaf)
                {
                    itemData.IsUpgraded = NumericHelpers.ParseBool(node.Value.Value) ?? false;
                }
            }

            return itemData;
        }
    }
}
