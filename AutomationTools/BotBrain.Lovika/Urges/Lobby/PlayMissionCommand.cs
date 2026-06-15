// ------------------------------------------------------------------------------
// <copyright file="PlayMissionCommand.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges.Lobby
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

    /// <summary>
    /// An Urge to issue the command to play a random mission from a supplied list
    /// </summary>
    public class PlayMissionCommand : LovikaLobbyUrge
    {
        private readonly ConcurrentDictionary<string, int> lastMissionIndex = new ConcurrentDictionary<string, int>();

        [JsonProperty]
        public bool Online { get; set; }

        [JsonProperty]
        public List<string> Missions { get; set; }

        protected override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (this.Missions == null || this.Missions.Count == 0)
            {
                return Task.FromResult(false);
            }

            return base.Update(state, bot);
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var lastMission = -1;

            if (this.lastMissionIndex.ContainsKey(bot.Name))
            {
                lastMission = this.lastMissionIndex[bot.Name];
            }

            var nextMission = this.GetNextMissionIndex(lastMission);
            var mission = this.Missions[nextMission];
            var command = this.GetConsoleCommand(mission);
            var action = new ConsoleCommandAction($"Start mission '{mission}'", command);
            action.OnRun += (object sender, LovikaBot e) => this.lastMissionIndex[bot.Name] = nextMission;
            yield return action;
        }

        private int GetNextMissionIndex(int current)
        {
            var next = current + 1;
            if (next >= this.Missions.Count)
            {
                next = 0;
            }

            return next;
        }

        // Start a mission with a given difficulty, threatlevel and an optional seed. (e.g. pumpkinpastures 2 5 1337)
        private string GetConsoleCommand(string mission) => $"Dungeons.Level.Start{(this.Online ? string.Empty : "Offline")} {mission}";
    }
}
