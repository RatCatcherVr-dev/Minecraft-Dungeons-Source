// ------------------------------------------------------------------------------
// <copyright file="TravelThroughDoors.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Cache;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor.Travel;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to and activate <see cref="InstantTravel"/> actors
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance), nameof(Parameters.Frequency), nameof(Parameters.SameFrequency))]
    public class TravelThroughDoors : LovikaLevelUrge
    {
        private readonly ConcurrentDictionary<string, DoorHistory> travelHistory = new ConcurrentDictionary<string, DoorHistory>();

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var player = state.PlayerCharacter(bot.PlayerIndex);

            if ((await state.GetAll<InstantTravel>(TimeSpan.FromSeconds(10)).ConfigureAwait(false))
                .Where(door => door.IsInRangeOf(player, maxDistance))
                .Where(door => !door.Blocked)
                .Any())
            {
                await state.GetVisibleContextWidgets(TimeSpan.Zero).ConfigureAwait(false);
                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var doorFrequency = this.GetParameter<TimeSpan>(bot, Parameters.Frequency);
            var sameDoorFrequency = this.GetParameter<TimeSpan>(bot, Parameters.SameFrequency);

            var doorHistory = this.travelHistory.GetOrAdd(bot.Name, new DoorHistory());

            var now = DateTime.Now;
            if (doorHistory.CanActivate(doorFrequency, now))
            {
                var player = state.PlayerCharacter(bot.PlayerIndex);
                var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

                var travels = state.ExistingAll<InstantTravel>()
                    .Where(door => door.IsInRangeOf(player, maxDistance))
                    .Where(door => !door.Blocked)
                    .Where(door => doorHistory.CanActivate(door, sameDoorFrequency, now));

                foreach (var travel in travels)
                {
                    if (this.IsWidgetForActor(visibleWidget, travel))
                    {
                        var action = new PlayerOnFunctionAction($"Activate {travel.Name} @{travel.Location()}", PlayerOnFunctionType.MeleeAttack);
                        action.OnRun += (object sender, LovikaBot e) => this.PlayerActivatedTravel(e, travel);
                        yield return action;
                    }
                    else
                    {
                        yield return new MoveToActorAction($"Move to {travel.Name} @{travel.Location()}", travel);
                    }
                }
            }
        }

        private void PlayerActivatedTravel(LovikaBot bot, InstantTravel instantTravel)
        {
            if (this.travelHistory.TryGetValue(bot.Name, out DoorHistory doorHistory))
            {
                doorHistory.UpdateActivated(instantTravel);
            }
        }

        private class DoorHistory
        {
            private readonly TimeElapsedHelper<InstantTravel> lastActivated = new TimeElapsedHelper<InstantTravel>();

            private DateTime activatedTime;

            public bool CanActivate(TimeSpan frequency, DateTime? now = null)
            {
                if (!now.HasValue)
                {
                    now = DateTime.Now;
                }

                return (this.activatedTime + frequency) <= now;
            }

            public bool CanActivate(InstantTravel instantTravel, TimeSpan frequency, DateTime? now = null)
            {
                return this.lastActivated.IsOlderThan(instantTravel, frequency, now);
            }

            public void UpdateActivated(InstantTravel instantTravel)
            {
                this.lastActivated.Refresh(instantTravel);
                this.activatedTime = DateTime.Now;
            }
        }
    }
}
