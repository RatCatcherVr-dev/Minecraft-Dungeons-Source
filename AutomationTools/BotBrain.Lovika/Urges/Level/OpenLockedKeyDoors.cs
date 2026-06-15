// ------------------------------------------------------------------------------
// <copyright file="OpenLockedKeyDoors.cs" company="Microsoft">
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
    using System.Runtime.CompilerServices;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Newtonsoft.Json;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to pick up keys, and open the doors that they open
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance))]
    public class OpenLockedKeyDoors : LovikaLevelUrge, IOnMapLoaded
    {
        private readonly string silverKeyCharacter = "BP_SilverBabyKeyCharacter_C";
        private readonly string goldKeyCharacter = "BP_GoldBabyKeyCharacter_C";

        private readonly string silverKeyItem = "BP_SilverBabyKey_Item_C";
        private readonly string goldKeyItem = "BP_GoldBabyKey_Item_C";

        [JsonProperty]
        public List<string> LockedDoors { get; set; }

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.ClearFailedAttempts(bot);
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);

            // FIXME When a mob has a key on there back this will also match
            var keyItems = await this.GetKeysItems(state, TimeSpan.Zero).ConfigureAwait(false);

            var keyCharacters = Enumerable.Empty<MobCharacter>();

            if (!keyItems.Any())
            {
                keyCharacters = await state.GetMobKeys(TimeSpan.Zero).ConfigureAwait(false);
            }

            if (keyCharacters.Any() || keyItems.Any())
            {
                var player = state.PlayerCharacter(bot.PlayerIndex);
                var doors = await this.GetDoorLockeds(bot, state, TimeSpan.FromSeconds(5)).ConfigureAwait(false);
                if (doors
                    .Where(d => d.InteractionEnabled)
                    .Where(d => d.IsInRangeOf(player, maxDistance))
                    .Any())
                {
                    await state.GetVisibleContextWidgets(TimeSpan.Zero).ConfigureAwait(false);
                }

                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var player = state.PlayerCharacter(bot.PlayerIndex);

            var keyItems = this.ExistingKeysItems(state);
            if (keyItems.Any())
            {
                var doorLockeds = this.ExistingDoorLockeds(state)
                    .Where(d => !d.IsOpen)
                    .Where(d => d.InteractionEnabled)
                    .OrderBy(k => k.DistanceTo(player));

                var keyItem = keyItems.First();
                var hasGoldKey = keyItem.Info.ClassName.Contains("Gold");

                var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

                foreach (var doorLocked in doorLockeds)
                {
                    if ((doorLocked.Info.ClassName.Contains("Gold") && hasGoldKey) ||
                        (!doorLocked.Info.ClassName.Contains("Gold") && !hasGoldKey))
                    {
                        if (this.IsWidgetForActor(visibleWidget, doorLocked))
                        {
                            var action = new PlayerOnFunctionAction($"Activate {doorLocked.Name}", PlayerOnFunctionType.MeleeAttack);
                            action.OnRun += (object sender, LovikaBot e) => state.ClearAll<DoorLocked>();
                            yield return action;
                        }
                        else
                        {
                            yield return new MoveToActorAction($"Move to {doorLocked.Name}", doorLocked);
                        }
                    }
                }
            }
            else
            {
                var keys = state.MobKeys()
                    .Where(k => k.IsInRangeOf(player, maxDistance))
                    .OrderBy(k => k.DistanceTo(player));

                var silverKeys = keys.Where(k => k.Info.ClassName == this.silverKeyCharacter);
                var goldKeys = keys.Where(k => k.Info.ClassName == this.goldKeyCharacter);

                if (silverKeys.Any())
                {
                    var key = silverKeys.First();
                    yield return new AttackMobAction($"Pickup key {key.Name}", key, false);
                }
                else if (goldKeys.Any())
                {
                    var key = goldKeys.First();
                    yield return new AttackMobAction($"Pickup key {key.Name}", key, false);
                }
            }
        }

        private async Task<IEnumerable<AActor>> GetKeysItems(LovikaGameState state, TimeSpan? maxAge = null)
        {
            var silverKeyItems = state.GetAll<AActor>(this.silverKeyItem, maxAge);
            var goldKeyItems = state.GetAll<AActor>(this.goldKeyItem, maxAge);

            var items = await Task.WhenAll(silverKeyItems, goldKeyItems).ConfigureAwait(false);
            return items.SelectMany(list => list);
        }

        private IEnumerable<AActor> ExistingKeysItems(LovikaGameState state)
        {
            var silverKeyItems = state.ExistingAll<AActor>(this.silverKeyItem);
            var goldKeyItems = state.ExistingAll<AActor>(this.goldKeyItem);

            return Enumerable.Concat(silverKeyItems, goldKeyItems);
        }

        private Task<IEnumerable<DoorLocked>> GetDoorLocked(string className, LovikaBot bot, LovikaGameState state, TimeSpan? maxAge = null)
        {
            if (this.CanAttemptGetAll(bot, className))
            {
                return this.AttemptGetAll<DoorLocked>(state, bot, className, maxAge);
            }

            return Task.FromResult(Enumerable.Empty<DoorLocked>());
        }

        private async Task<IEnumerable<DoorLocked>> GetDoorLockeds(LovikaBot bot, LovikaGameState state, TimeSpan? maxAge = null)
        {
            var getDoors = this.LockedDoors.Select(className => this.GetDoorLocked(className, bot, state, maxAge));
            var doors = await Task.WhenAll(getDoors).ConfigureAwait(false);
            return doors.SelectMany(list => list);
        }

        private IEnumerable<DoorLocked> ExistingDoorLockeds(LovikaGameState state)
        {
            var existingDoors = this.LockedDoors.Select(className => state.ExistingAll<DoorLocked>(className));
            return existingDoors.SelectMany(list => list);
        }
    }
}
