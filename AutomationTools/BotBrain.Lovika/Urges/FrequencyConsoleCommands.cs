// ------------------------------------------------------------------------------
// <copyright file="FrequencyConsoleCommands.cs" company="Microsoft">
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
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Newtonsoft.Json;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge that runs console commands periodically
    /// </summary>
    [UsesParameter(nameof(Parameters.Frequency))]
    public class FrequencyConsoleCommands : LovikaInGameUrge
    {
        private readonly TimeElapsedHelper<LovikaBot> timeElapsed = new TimeElapsedHelper<LovikaBot>();

        [JsonProperty]
        public List<string> Commands { get; set; }

        [JsonProperty]
        public string World { get; set; }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if (!state.ExistingAll<UWorld>().Where(w => w.Name == this.World).Any())
            {
                return false;
            }

            var frequency = this.GetParameter<TimeSpan>(bot, Parameters.Frequency);
            return this.timeElapsed.IsOlderThan(bot, frequency);
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var action = new ConsoleCommandAction($"Console command '{string.Join("', '", this.Commands)}'", this.Commands.ToArray());
            action.OnRun += (object sender, LovikaBot e) => this.timeElapsed.Refresh(e);
            yield return action;
        }
    }
}
