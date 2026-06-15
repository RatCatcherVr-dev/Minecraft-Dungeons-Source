// ------------------------------------------------------------------------------
// <copyright file="PickupItems.cs" company="Microsoft">
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
    using System.Text.RegularExpressions;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor.Item;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to and pick up <see cref="StorableItem"/>.
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance), nameof(Parameters.RegexFilters))]
    public class PickupItems : LovikaInGameUrge, IOnMapLoaded
    {
        public void OnMapLoaded(LovikaGameState state, LovikaBot bot) => state.ClearAll<StorableItem>();

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var player = state.PlayerCharacter(bot.PlayerIndex);

            if ((await state.GetAll<StorableItem>(TimeSpan.Zero).ConfigureAwait(false))
                .Where(i => i.ItemId != "Emerald")
                .Where(i => i.IsInRangeOf(player, maxDistance))
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
            var regexFilters = this.GetParameter<string[]>(bot, Parameters.RegexFilters)
                .Select(s => new Regex(s));

            var player = state.PlayerCharacter(bot.PlayerIndex);
            var items = state.ExistingAll<StorableItem>()
                .Where(i => i.ItemId != "Emerald")
                .Where(i => i.IsInRangeOf(player, maxDistance))
                .Where(i => this.IsValidFilter(i, regexFilters));

            var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

            bool hasPickupAction = false;
            foreach (var item in items)
            {
                if (this.CanPickUp(item, visibleWidget, item.DistanceTo(player), out var currentItem))
                {
                    if (hasPickupAction)
                    {
                        continue;
                    }

                    hasPickupAction = true;
                    yield return new PlayerOnFunctionAction($"Pickup item {currentItem.Name}", PlayerOnFunctionType.MeleeAttack);
                }
                else
                {
                    yield return new MoveToActorAction($"Move to item {item.Name}", item, acceptanceRadius: 10);
                }
            }
        }

        private bool CanPickUp(StorableItem item, ContextSensitiveWidget widget, float distance, out StorableItem currentItem)
        {
            currentItem = null;
            if (widget == null)
            {
                return false;
            }

            var isCurrentItem = widget.InteractableActorInfo.Equals(item.Info);
            if (isCurrentItem)
            {
                currentItem = item;
            }
            else
            {
                var actor = widget.InteractableActor<AActor>();
                var storable = actor as StorableItem;
                if (storable == null)
                {
                    return false;
                }

                currentItem = storable;
            }

            if (currentItem != null && currentItem.LockItemToOwner)
            {
                // TODO If the item is owned by another player then don't pickup (co-op/online only)
            }

            return isCurrentItem || distance <= 100;
        }

        private bool IsValidFilter(StorableItem item, IEnumerable<Regex> filters)
        {
            if (filters == null)
            {
                return true;
            }

            if (!filters.Any())
            {
                return true;
            }

            var valid = false;
            foreach (var filter in filters)
            {
                if (filter.IsMatch(item.ItemId))
                {
                    valid = true;
                    break;
                }
            }

            return valid;
        }
    }
}
