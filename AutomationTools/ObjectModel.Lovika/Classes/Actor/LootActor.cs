// ------------------------------------------------------------------------------
// <copyright file="LootActor.cs" company="Microsoft">
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

    [GameClass("LootActor")]
    [GameProperty("bLootUnlocked", "SkeletalMesh.bVisible")]
    public class LootActor : PropActor
    {
        public LootActor(IGameObject obj) : base(obj)
        {
        }

        public bool LootUnlocked => NumericHelpers.ParseBool(this["bLootUnlocked"] as string) ?? false;

        public bool SkeletalMeshVisible => NumericHelpers.ParseBool(this["SkeletalMesh.bVisible"] as string) ?? true;
    }
}
