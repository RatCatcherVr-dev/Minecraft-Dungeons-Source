// ------------------------------------------------------------------------------
// <copyright file="OpenSupplyStations.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Player;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to and open a supply chest
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance))]
    public class OpenSupplyStations : LovikaLevelUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var supplies = await state.GetAll<SupplyStation>(TimeSpan.FromSeconds(10)).ConfigureAwait(false);
            if (supplies.Any())
            {
                var player = state.PlayerCharacter(bot.PlayerIndex);
                await state.GetVisibleContextWidgetsWithOwnership(player, TimeSpan.Zero).ConfigureAwait(false);
                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);

            var playerCount = state.ExistingAll<PlayerPawn>()
                .Select(p => p.Info)
                .Distinct()
                .Count();
            var player = state.PlayerCharacter(bot.PlayerIndex);

            // TODO This needs further work when we have multiple players. Each player can only open the supply chest once
            var supplies = state.ExistingAll<SupplyStation>()
                .Where(station => station.IsInRangeOf(player, maxDistance))
                .Where(station => station.OpenCount < playerCount);

            var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

            foreach (var station in supplies)
            {
                if (this.IsWidgetForActor(visibleWidget, station))
                {
                    var action = new PlayerOnFunctionAction($"Open supply station {station.Name}", PlayerOnFunctionType.MeleeAttack);
                    action.OnRun += (object sender, LovikaBot e) => state.ClearAll<SupplyStation>();
                    yield return action;
                }
                else
                {
                    yield return new MoveToActorAction($"Move to supply station {station.Name} @{station.Location()}", station);
                }
            }
        }
    }
}
