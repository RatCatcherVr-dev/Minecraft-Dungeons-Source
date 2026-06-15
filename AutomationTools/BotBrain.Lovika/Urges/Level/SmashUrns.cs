// ------------------------------------------------------------------------------
// <copyright file="SmashUrns.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to and smash urns
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance))]
    public class SmashUrns : LovikaLevelUrge, IOnMapLoaded
    {
        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.ClearFailedAttempts(bot);
            state.ClearAll<LootUrn>();
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!this.CanAttemptGetAll<LootUrn>(bot))
            {
                return false;
            }

            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var player = state.PlayerCharacter(bot.PlayerIndex);

            if ((await this.AttemptGetAll<LootUrn>(state, bot, TimeSpan.FromSeconds(15)).ConfigureAwait(false))
                .Where(u => u.IsInRangeOf(player, maxDistance))
                .Where(u => u.InteractionEnabled)
                .Any())
            {
                await state.GetAll<ContextSensitiveWidget>(TimeSpan.Zero).ConfigureAwait(false);
                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);

            var player = state.PlayerCharacter(bot.PlayerIndex);
            var playerLocation = player.Location().Value;

            var urns = state.ExistingAll<LootUrn>()
                .Where(u => u.IsInRangeOf(player, maxDistance))
                .Where(u => u.InteractionEnabled);

            var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

            foreach (var urn in urns)
            {
                if (this.IsWidgetForActor(visibleWidget, urn))
                {
                    var action = new PlayerOnFunctionAction($"Smash urn {urn.Name}", PlayerOnFunctionType.MeleeAttack);
                    action.OnRun += (object sender, LovikaBot e) => state.ClearAll<LootUrn>();
                    yield return action;
                }
                else
                {
                    yield return new MoveToActorAction($"Move to urn {urn.Name} @{urn.Location()}", urn);
                }
            }
        }
    }
}
