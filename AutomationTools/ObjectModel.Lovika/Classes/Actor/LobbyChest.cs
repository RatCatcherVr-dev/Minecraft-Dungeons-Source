// ------------------------------------------------------------------------------
// <copyright file="LobbyChest.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    [GameClass("LobbyChest")]
    [GameProperty("ChestType", "bLootUnlocked")]
    public class LobbyChest : AActor
    {
        public LobbyChest(IGameObject obj) : base(obj)
        {
        }

        public LobbyChestType ChestType => (LobbyChestType)Enum.Parse(typeof(LobbyChestType), this["ChestType"] as string, true);

        public bool LootUnlocked => NumericHelpers.ParseBool(this["bLootUnlocked"] as string) ?? false;
    }
}
