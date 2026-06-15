// ------------------------------------------------------------------------------
// <copyright file="SetGamepadActiveAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading.Tasks;

    public class SetGamepadActiveAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot to set the gamepad state.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="active">The states of the players gamepad.</param>
        public SetGamepadActiveAction(string name, bool active) : base(name)
        {
            this.Active = active;
            this.Action = this.SetPlayerGamepadActive;
        }

        /// <summary>
        /// The state of the players gamepad.
        /// </summary>
        [DataMember]
        public bool Active { get; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as SetGamepadActiveAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Active == action.Active;
        }

        /// <summary>
        /// Sets the players gamepad flag
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task SetPlayerGamepadActive(LovikaBot bot)
        {
            return bot.Client.SetGamepadActive(bot.PlayerIndex, this.Active);
        }
    }
}
