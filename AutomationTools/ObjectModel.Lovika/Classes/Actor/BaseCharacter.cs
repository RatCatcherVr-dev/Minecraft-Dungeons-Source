// ------------------------------------------------------------------------------
// <copyright file="BaseCharacter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.GameToolkit;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    [GameClass("BaseCharacter")]
    [GameProperty("rotation", "Targetable", "WorldState", "TeamName", "HealthComponent.Shield", "HealthComponent.Health", "HealthComponent.MaxHealth", "HealthComponent.ResistDeath")]
    public class BaseCharacter : APawn
    {
        public BaseCharacter(IGameObject obj) : base(obj)
        {
        }

        public bool IsTargetable => NumericHelpers.ParseBool(this["Targetable"] as string) ?? true;

        public string WorldState => this["WorldState"] as string;

        public object Shield => this["HealthComponent.Shield"];
        public object Health => this["HealthComponent.Health"];
        public object MaxHealth => this["HealthComponent.MaxHealth"];
        public object ResistDeath => this["HealthComponent.ResistDeath"];

        public TeamName TeamName => (TeamName)Enum.Parse(typeof(TeamName), this["TeamName"] as string, true);

        public bool IsInWorld() => this.WorldState == "InWorld";

        public Vector3 GetForwardVector()
        {
            var rotation = this.Rotation().Value;
            var x = Math.Cos(rotation.Pitch) * Math.Cos(rotation.Yaw);
            var y = Math.Cos(rotation.Pitch) * Math.Sin(rotation.Yaw);
            var z = Math.Sin(rotation.Pitch);
            return new Vector3((float)x, (float)y, (float)z);
        }
    }
}
