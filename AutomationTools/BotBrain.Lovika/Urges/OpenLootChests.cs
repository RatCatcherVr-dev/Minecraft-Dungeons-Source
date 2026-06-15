// ------------------------------------------------------------------------------
// <copyright file="OpenLootChests.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to and open a <see cref="LootActor"/>
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance), nameof(Parameters.Hidden))]
    public class OpenLootChests : LovikaInGameUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if ((await state.GetAll<LootActor>(TimeSpan.FromSeconds(10)).ConfigureAwait(false))
                .Where(loot => !loot.LootUnlocked)
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
            var allowHidden = this.GetParameter<bool>(bot, Parameters.Hidden);

            var player = state.PlayerCharacter(bot.PlayerIndex);

            var lootList = state.ExistingAll<LootActor>()
                .Where(loot => !loot.LootUnlocked)
                .Where(loot => loot.IsInRangeOf(player, maxDistance))
                .Where(loot => allowHidden || loot.SkeletalMeshVisible);

            var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

            foreach (var loot in lootList)
            {
                if (this.IsWidgetForActor(visibleWidget, loot))
                {
                    var action = new PlayerOnFunctionAction($"Open loot {loot.Name}", PlayerOnFunctionType.MeleeAttack);
                    action.OnRun += (object sender, LovikaBot e) => state.ClearAll<LootActor>();
                    yield return action;
                }
                else
                {
                    yield return new MoveToActorAction($"Move to loot {loot.Name}", loot);
                }
            }
        }
    }
}
