// ------------------------------------------------------------------------------
// <copyright file="ActivateRandomSlot.cs" company="Microsoft">
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
    /// An urge to activate a random slot periodically.
    /// </summary>
    [UsesParameter(nameof(Parameters.Frequency))]
    public class ActivateRandomSlot : LovikaInGameUrge
    {
        private readonly TimeElapsedHelper<LovikaBot> timeElapsed = new TimeElapsedHelper<LovikaBot>();

        private readonly int maxSlot = 3;

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
            var slot = this.GetRandomSlot();
            var action = new PlayerOnFunctionAction($"Activate Slot {slot}", PlayerOnFunctionType.ActivateSlot, slot);

            action.OnRun += (object sender, LovikaBot e) => this.timeElapsed.Refresh(e);
            yield return action;
        }

        private int GetRandomSlot()
        {
            var random = new Random((int)DateTime.UtcNow.Ticks);
            return random.Next(0, this.maxSlot);
        }
    }
}
