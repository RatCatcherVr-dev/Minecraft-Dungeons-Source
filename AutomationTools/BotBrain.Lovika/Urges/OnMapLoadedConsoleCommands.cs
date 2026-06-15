// ------------------------------------------------------------------------------
// <copyright file="OnMapLoadedConsoleCommands.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Newtonsoft.Json;

    public class OnMapLoadedConsoleCommands : LovikaInGameUrge, IOnMapLoaded
    {
        private readonly ConcurrentDictionary<string, bool> readyToIssue = new ConcurrentDictionary<string, bool>();

        [JsonProperty]
        public List<string> Commands { get; set; }

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.readyToIssue[bot.Name] = true;
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if (!this.readyToIssue.ContainsKey(bot.Name))
            {
                return true;
            }

            return this.readyToIssue[bot.Name];
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var action = new ConsoleCommandAction($"Console command(s) '{string.Join("', '", this.Commands)}'", this.Commands.ToArray());
            action.OnRun += (object sender, LovikaBot e) => this.readyToIssue[e.Name] = false;
            yield return action;
        }
    }
}
