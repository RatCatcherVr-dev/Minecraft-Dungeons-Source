// ------------------------------------------------------------------------------
// <copyright file="ActivateBeacons.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk and activate <see cref="GuideBeacon"/>
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance))]
    public class ActivateBeacons : LovikaLevelUrge, IOnMapLoaded
    {
        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.ClearFailedAttempts(bot);
            state.ClearAll<GuideBeacon>();
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!this.CanAttemptGetAll<GuideBeacon>(bot))
            {
                return false;
            }

            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var player = state.PlayerCharacter(bot.PlayerIndex);

            if ((await this.AttemptGetAll<GuideBeacon>(state, bot, TimeSpan.FromSeconds(5)).ConfigureAwait(false))
                .Where(b => b.IsInRangeOf(player, maxDistance))
                .Where(b => b.InteractionEnabled)
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

            var player = state.PlayerCharacter(bot.PlayerIndex);
            var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

            var beacons = state.ExistingAll<GuideBeacon>()
                .Where(b => b.IsInRangeOf(player, maxDistance))
                .Where(b => b.InteractionEnabled);

            foreach (var beacon in beacons)
            {
                var location = beacon.Location().Value;
                if (this.IsWidgetForActor(visibleWidget, beacon))
                {
                    var action = new PlayerOnFunctionAction($"Activate {beacon.Name}", PlayerOnFunctionType.MeleeAttack);
                    action.OnRun += (object sender, LovikaBot e) => state.ClearAll<GuideBeacon>();
                    yield return action;
                }
                else
                {
                    var dist = player.DistanceTo(location);
                    if (dist <= 250)
                    {
                        var action = new PlayerOnFunctionAction($"Activate close to {beacon.Name}", PlayerOnFunctionType.MeleeAttack);
                        action.OnRun += (object sender, LovikaBot e) => state.ClearAll<GuideBeacon>();
                        yield return action;
                    }
                    else
                    {
                        yield return new MoveToActorAction($"Move to {beacon.Name}", beacon);
                    }
                }
            }
        }
    }
}
