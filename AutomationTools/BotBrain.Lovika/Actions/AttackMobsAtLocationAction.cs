// ------------------------------------------------------------------------------
// <copyright file="AttackMobsAtLocationAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    [ActionWithWorldPosition("Location")]
    public class AttackMobsAtLocationAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot to attack a team at and around a specified <see cref="Vector3"/>.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="location">The location to move to and attack from.</param>
        /// <param name="attackableTeam">The team to attack.</param>
        /// <param name="radius">The attack radius.</param>
        /// <param name="allowRanged">If true, ranged attacks are allowed.</param>
        public AttackMobsAtLocationAction(string name, Vector3 location, TeamName attackableTeam, float radius, bool allowRanged = false) : base(name)
        {
            this.Location = location;
            this.AttackableTeam = attackableTeam;
            this.AttackRadius = radius;
            this.AllowRanged = allowRanged;
            this.Action = this.AttackAtLocation;
        }

        /// <summary>
        /// Gets the location to move to and attack from.
        /// </summary>
        [DataMember]
        public Vector3 Location { get; }

        /// <summary>
        /// Gets the team to attack.
        /// </summary>
        [DataMember]
        public TeamName AttackableTeam { get; }

        /// <summary>
        /// Gets the attack radius.
        /// </summary>
        [DataMember]
        public float AttackRadius { get; }

        /// <summary>
        /// Gets if ranged attacks may be used.
        /// </summary>
        [DataMember]
        public bool AllowRanged { get; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as AttackMobsAtLocationAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Location == action.Location
                && this.AttackableTeam == action.AttackableTeam
                && this.AttackRadius == action.AttackRadius
                && this.AllowRanged == action.AllowRanged;
        }

        /// <summary>
        /// Tells the player to attack any actors in a radius around a location.
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task AttackAtLocation(LovikaBot bot)
        {
            return bot.Client.PlayerAttackRadiusAttack(bot.PlayerIndex, this.Name, this.Location, this.AttackableTeam, this.AttackRadius, this.AllowRanged);
        }
    }
}
