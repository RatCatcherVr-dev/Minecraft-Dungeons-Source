// ------------------------------------------------------------------------------
// <copyright file="AttackMobAction.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;

    [ActionWithWorldPosition("Location")]
    public class AttackMobAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot to attack a single <see cref="BaseCharacter"/>.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="actor">The actor to move the player to.</param>
        /// <param name="allowRanged">If true, ranged attacks are allowed.</param>
        public AttackMobAction(string name, BaseCharacter actor, bool allowRanged = false) : base(name)
        {
            this.Actor = actor;
            this.AllowRanged = allowRanged;
            this.Action = this.AttackActor;
        }

        /// <summary>
        /// Gets the actor to target attack.
        /// </summary>
        public BaseCharacter Actor { get; }

        /// <summary>
        /// Gets the location of the actor.
        /// </summary>
        [DataMember]
        public Vector3? Location => this.Actor.Location();

        /// <summary>
        /// Gets if ranged attacks may be used.
        /// </summary>
        [DataMember]
        public bool AllowRanged { get; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as AttackMobAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Actor == action.Actor
                && this.Location == action.Location
                && this.AllowRanged == action.AllowRanged;
        }

        /// <summary>
        /// Tells the player to attack a certain actor.
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task AttackActor(LovikaBot bot)
        {
            return bot.Client.PlayerAttackMobAttack(bot.PlayerIndex, this.Name, this.Actor, this.AllowRanged);
        }
    }
}
