// ------------------------------------------------------------------------------
// <copyright file="ActivateHealthPotion.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Cache;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An urge to activate the health potion periodically.
    /// </summary>
    [UsesParameter(nameof(Parameters.Frequency))]
    public class ActivateHealthPotion : LovikaInGameUrge
    {
        private readonly TimeElapsedHelper<LovikaBot> timeElapsed = new TimeElapsedHelper<LovikaBot>();

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var frequency = this.GetParameter<TimeSpan>(bot, Parameters.Frequency);
            return this.timeElapsed.IsOlderThan(bot, frequency);
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var action = new PlayerOnFunctionAction("Activate Health Potion", PlayerOnFunctionType.ActivateHealthSlot);
            action.OnRun += (object sender, LovikaBot e) => this.timeElapsed.Refresh(e);
            yield return action;
        }
    }
}
