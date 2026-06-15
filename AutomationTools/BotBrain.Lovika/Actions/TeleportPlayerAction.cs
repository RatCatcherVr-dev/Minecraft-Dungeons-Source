// ------------------------------------------------------------------------------
// <copyright file="TeleportPlayerAction.cs" company="Microsoft">
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
    using Microsoft.GNS.GameToolkit;

    [ActionWithWorldPosition("Location")]
    public class TeleportPlayerAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot to teleport the player to a given <see cref="Vector3"/>.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="location">The location to teleport the player to</param>
        /// <param name="rotaion">The rotation to place the player in.</param>
        /// <param name="enforceNotClipping">
        /// Indicates if the teleport should be done no matter what.
        /// If set to true the engine will look for and adjust or fail due to clipping issues.
        /// If set to false, clipping issues may happen.
        /// </param>
        public TeleportPlayerAction(string name, Vector3 location, Rotation3 rotaion = default, bool enforceNotClipping = true) : base(name)
        {
            this.Location = location;
            this.Rotation = rotaion;
            this.EnforceNotClipping = enforceNotClipping;
            this.Action = this.TeleportPlayer;
        }

        /// <summary>
        /// Gets the location to teleport to.
        /// </summary>
        [DataMember]
        public Vector3 Location { get; }

        /// <summary>
        /// Gets the rotation to use.
        /// </summary>
        [DataMember]
        public Rotation3 Rotation { get; }

        /// <summary>
        /// Gets if the teleport should be done no matter what. If set to true the engine
        /// will look for and adjust or fail due to clipping issues. If set to false, clipping
        /// issues may happen.
        /// </summary>
        [DataMember]
        public bool EnforceNotClipping { get; }

        /// <inheritdoc/>
        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as TeleportPlayerAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Location == action.Location
                && Rotation3.AreEqual(this.Rotation, action.Rotation, 0f)
                && this.EnforceNotClipping == action.EnforceNotClipping;
        }

        /// <summary>
        /// Teleports the player with the specified index to a certain location and rotation in the current map.
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task TeleportPlayer(LovikaBot bot)
        {
            return Task.Run(() =>
            {
                bot.Client.TeleportPlayer(bot.PlayerIndex, this.Location, this.Rotation, this.EnforceNotClipping);
            });
        }
    }
}
