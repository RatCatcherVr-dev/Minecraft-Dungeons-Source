// ------------------------------------------------------------------------------
// <copyright file="ConsoleCommandAction.cs" company="Microsoft">
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

    public class ConsoleCommandAction : BotActionDelegate
    {
        /// <summary>
        /// A class representing bot actions which execute a console command.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="commands">The console commands to use.</param>
        public ConsoleCommandAction(string name, params string[] commands) : base(name)
        {
            this.Commands = commands.ToList();
            this.Action = this.ExecuteConsoleCommand;
        }

        /// <summary>
        /// Gets the console commands.
        /// </summary>
        [DataMember]
        public List<string> Commands { get; }

        /// <inheritdoc/>
        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as ConsoleCommandAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Commands.SequenceEqual(action.Commands);
        }

        /// <summary>
        /// Executes the console command.
        /// </summary>
        /// <param name="bot">The LovikaBot bot to execute the command.</param>
        /// <returns>The task to await.</returns>
        private Task ExecuteConsoleCommand(LovikaBot bot)
        {
            return Task.Run(() =>
            {
                foreach (var command in this.Commands)
                {
                    bot.Client.ExecuteCommand(command);
                }
            });
        }
    }
}
