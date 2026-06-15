// ------------------------------------------------------------------------------
// <copyright file="BotActionDelegate.cs" company="Microsoft">
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

    public class BotActionDelegate : LovikaBotActionBase
    {
        /// <summary>
        /// A class representing bot actions which call a delegate function with the bot.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="action">The delegate to invoke.</param>
        public BotActionDelegate(string name, Func<LovikaBot, Task<bool>> action = null)
        {
            this.Name = name;
            this.Action = action;
        }

        /// <summary>
        /// Occurs when the action is running.
        /// </summary>
        public event EventHandler<LovikaBot> OnRun;

        /// <summary>
        /// Gets or sets the delegate action to invoke.
        /// </summary>
        public Func<LovikaBot, Task> Action { get; protected set; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as BotActionDelegate;
            return action != null
                && this.ToString() == action.ToString()
                && this.Action == action.Action;
        }

        public override async Task Run(LovikaBot bot)
        {
            if (this.Action != null)
            {
                try
                {
                    await this.Action(bot).ConfigureAwait(false);
                }
                catch
                {
                    this.Status = BotActionStatus.Error;
                    throw;
                }

                this.OnRun?.Invoke(this, bot);
            }
        }
    }
}
