// ------------------------------------------------------------------------------
// <copyright file="PlayerOnFunctionAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    [UninterruptibleAction(2500)]
    public class PlayerOnFunctionAction : BotActionDelegate
    {
        /// <summary>
        /// Tells the bot the call the specified <see cref="PlayerOnFunctionType"/> on the player.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="type">The action type.</param>
        /// <param name="index">The action index to use, if the action supports an index.</param>
        public PlayerOnFunctionAction(string name, PlayerOnFunctionType type, int index = 0) : base(name)
        {
            this.Action = this.PlayerOnFunction;
            this.Type = type;
            this.Index = index;
        }

        public PlayerOnFunctionType Type { get; }

        public int Index { get; }

        /// <summary>
        /// Sends a player on function action to the player
        /// </summary>
        /// <param name="bot">The Lovika bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task PlayerOnFunction(LovikaBot bot)
        {
            return bot.Client.PlayerOnFunctionAction(bot.PlayerIndex, this.Name, this.Type, this.Index);
        }
    }
}
