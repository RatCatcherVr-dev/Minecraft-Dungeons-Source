// ------------------------------------------------------------------------------
// <copyright file="PlayerInputAction.cs" company="Microsoft">
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

    [UninterruptibleAction(1500)]
    public class PlayerInputAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot to press and release an input key.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="inputKey">The input key to press.</param>
        /// <param name="useViewport">If the key should pressed on the viewport or via the controller.</param>
        public PlayerInputAction(string name, string inputKey, bool useViewport = false) : base(name)
        {
            this.InputKey = inputKey;
            this.Viewport = useViewport;
            this.Action = this.PlayerRawInputKeyPressed;
        }

        /// <summary>
        /// The input key to press.
        /// </summary>
        [DataMember]
        public string InputKey { get; }

        [DataMember]
        public bool Viewport { get; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as PlayerInputAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.InputKey == action.InputKey
                && this.Viewport == action.Viewport;
        }

        /// <summary>
        /// Sends a input key to the player
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task PlayerRawInputKeyPressed(LovikaBot bot)
        {
            return bot.Client.PlayerInputKey(bot.PlayerIndex, this.Name, this.InputKey, this.Viewport);
        }
    }
}
