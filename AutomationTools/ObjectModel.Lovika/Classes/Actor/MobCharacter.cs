// ------------------------------------------------------------------------------
// <copyright file="MobCharacter.cs" company="Microsoft">
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

    [GameClass("MobCharacter")]
    [GameProperty("OffensiveRange", "mPlayerDistanceCheck", "mPlayerVisible", "EntityType")]
    public class MobCharacter : BaseCharacter
    {
        public MobCharacter(IGameObject obj) : base(obj)
        {
        }

        public float OffensiveRange => NumericHelpers.ParseFloat(this["OffensiveRange"] as string) ?? 1500;

        public bool PlayerDistanceCheck => NumericHelpers.ParseBool(this["mPlayerDistanceCheck"] as string) ?? true;

        public bool PlayerVisible => NumericHelpers.ParseBool(this["mPlayerVisible"] as string) ?? true;

        public string EntityType => this["EntityType"] as string;
    }
}
