// ------------------------------------------------------------------------------
// <copyright file="MoveToActorAction.cs" company="Microsoft">
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
    using Microsoft.GNS.GameStateTracker.UE4;

    [ActionWithWorldPosition("Location")]
    public class MoveToActorAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot to move to an <see cref="AActor"/>.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="actor">The actor to move the player to</param>
        /// <param name="pathFinding">If set regular path finding will be used, if not then direct path between two points.</param>
        /// <param name="acceptanceRadius">The required distance to location to complete move.</param>
        public MoveToActorAction(string name, AActor actor, bool pathFinding = true, float acceptanceRadius = -1.0f) : base(name)
        {
            this.Actor = actor;
            this.PathFinding = pathFinding;
            this.AcceptanceRadius = acceptanceRadius;
            this.Action = this.MovePlayer;
        }

        /// <summary>
        /// Gets the actor to move to.
        /// </summary>
        public AActor Actor { get; }

        /// <summary>
        /// Gets the location of the actor.
        /// </summary>
        [DataMember]
        public Vector3? Location => this.Actor.Location();

        /// <summary>
        /// Gets if regular pathfinding will be used, if not direct path between two points
        /// </summary>
        [DataMember]
        public bool PathFinding { get; }

        /// <summary>
        /// Gets the required distance to location to complete move
        /// </summary>
        [DataMember]
        public float AcceptanceRadius { get; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as MoveToActorAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Actor == action.Actor
                && this.Location == action.Location
                && this.PathFinding == action.PathFinding
                && this.AcceptanceRadius == action.AcceptanceRadius;
        }

        /// <summary>
        /// Moves the player with the specified index to a certain location in the map
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task MovePlayer(LovikaBot bot)
        {
            return bot.Client.MovePlayerToActor(bot.PlayerIndex, this.Name, this.Actor, this.PathFinding, this.AcceptanceRadius);
        }
    }
}
