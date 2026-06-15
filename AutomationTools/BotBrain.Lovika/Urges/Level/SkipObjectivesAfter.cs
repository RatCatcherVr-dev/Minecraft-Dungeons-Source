// ------------------------------------------------------------------------------
// <copyright file="SkipObjectivesAfter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Cache;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;

    /// <summary>
    /// A cheat urge to skip objectives after a specified time
    /// </summary>
    [UsesParameter(nameof(Parameters.Frequency))]
    public class SkipObjectivesAfter : ActivateObjective, IOnMapLoaded
    {
        private readonly TimeElapsedHelper<LovikaBot> firstRun = new TimeElapsedHelper<LovikaBot>();
        private readonly ConcurrentDictionary<string, IEnumerable<Vector3>> objectivesCache = new ConcurrentDictionary<string, IEnumerable<Vector3>>();

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot) => this.ClearStoredCache(bot);

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                var gameBP = state.ExistingAll<GameBP>().FirstOrDefault();
                if (gameBP != null)
                {
                    if (!gameBP.ObjectiveLocations.Locations.Any())
                    {
                        this.ClearStoredCache(bot);
                    }
                }

                return false;
            }

            var currentLocations = state.ExistingAll<GameBP>().First().ObjectiveLocations.Locations;
            var cacheLocations = this.objectivesCache.GetOrAdd(bot.Name, currentLocations);

            if (!cacheLocations.SequenceEqual(currentLocations))
            {
                this.ClearStoredCache(bot);
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
            var gameBp = state.ExistingAll<GameBP>().First();
            var noObjectives = gameBp.ObjectiveLocations.Locations.Count();

            var action = new ConsoleCommandAction("Issue Skip Objectives Command", $"Dungeons.Player.SkipObjectives {noObjectives}")
            {
                IsWorkaround = true
            };
            action.OnRun += (object sender, LovikaBot e) => this.firstRun.Remove(e);
            yield return action;
        }

        private void ClearStoredCache(LovikaBot bot)
        {
            this.firstRun.Remove(bot);
            this.objectivesCache.TryRemove(bot.Name, out IEnumerable<Vector3> _);
        }
    }
}
