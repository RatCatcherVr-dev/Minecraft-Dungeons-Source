// ------------------------------------------------------------------------------
// <copyright file="WinAfter.cs" company="Microsoft">
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
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// A cheat urge to issue the win command after a specified time
    /// </summary>
    [UsesParameter(nameof(Parameters.Frequency))]
    public class WinAfter : LovikaLevelUrge, IOnMapLoaded
    {
        private readonly TimeElapsedHelper<LovikaBot> firstRun = new TimeElapsedHelper<LovikaBot>();

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.firstRun.Remove(bot);
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var now = DateTime.UtcNow;
            if (!this.firstRun.TryGet(bot, now).HasValue)
            {
                this.firstRun.Set(bot, now);
                return false;
            }

            var frequency = this.GetParameter<TimeSpan>(bot, Parameters.Frequency);
            return this.firstRun.IsOlderThan(bot, frequency, now);
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var action = new ConsoleCommandAction("Issue Win Command", "Dungeons.Player.Win")
            {
                IsWorkaround = true
            };
            action.OnRun += (object sender, LovikaBot e) => this.firstRun.Remove(e);
            yield return action;
        }
    }
}
