// ------------------------------------------------------------------------------
// <copyright file="OpenLobbyChests.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;

    /// <summary>
    /// An Urge to walk to and open <see cref="LobbyChest"/>
    /// </summary>
    public class OpenLobbyChests : LovikaLobbyUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if ((await state.GetAll<LobbyChest>(TimeSpan.Zero).ConfigureAwait(false))
                .Where(chest => chest.ChestType == LobbyChestType.Chest)
                .Where(chest => !chest.LootUnlocked)
                .Any())
            {
                await state.GetVisibleContextWidgets(TimeSpan.Zero).ConfigureAwait(false);
                await state.GetAll<UserWidget>("UMG_ChestNotificationWidget_C", TimeSpan.Zero).ConfigureAwait(false);
                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var notification = state.ExistingAll<UserWidget>("UMG_ChestNotificationWidget_C")
                .Where(widget => widget.IsValidObject())
                .Where(widget => widget.IsVisible())
                .FirstOrDefault();

            if (notification != null)
            {
                // TODO Make it only the player who opened the chest can do anything here
                yield return new PlayerInputAction($"Open Chest", "Gamepad_FaceButton_Bottom");
            }
            else
            {
                var maxDistance = this.GetParameter<float>(bot, Resources.BotBrains.Parameters.MaxDistance);

                var player = state.PlayerCharacter(bot.PlayerIndex);

                var chestList = state.ExistingAll<LobbyChest>()
                    .Where(chest => chest.ChestType == LobbyChestType.Chest)
                    .Where(loot => !loot.LootUnlocked);

                var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

                foreach (var chest in chestList)
                {
                    if (this.IsWidgetForActor(visibleWidget, chest))
                    {
                        yield return new PlayerOnFunctionAction($"Open lobby chest {chest.Name}", PlayerOnFunctionType.MeleeAttack);
                    }
                    else
                    {
                        yield return new MoveToActorAction($"Move to lobby chest {chest.Name} @{chest.Location()}", chest);
                    }
                }
            }
        }
    }
}
