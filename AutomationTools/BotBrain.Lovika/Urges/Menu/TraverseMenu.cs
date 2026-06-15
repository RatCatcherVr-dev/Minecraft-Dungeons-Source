// ------------------------------------------------------------------------------
// <copyright file="TraverseMenu.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using DeviceConsole.Environments.Input;
    using DeviceConsole.Xbox;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;

    /// <summary>
    /// An Urge to traverse the main menu to get the bot into the lobby
    /// </summary>
    public class TraverseMenu : LovikaMenuUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var titleScreens = (await state.GetAll<TitleScreen>(TimeSpan.Zero).ConfigureAwait(false))
                .Where(w => w.IsValidObject() && w.IsVisible());
            if (titleScreens.Any())
            {
                var titleScreen = titleScreens.First();
                var button = await titleScreen.GetPressAnyButton(TimeSpan.Zero).ConfigureAwait(false);
                if (button.IsValidObject() && button.IsVisible())
                {
                    return true;
                }
            }

            if ((await state.GetAll<Menu>(TimeSpan.Zero).ConfigureAwait(false))
                .Where(w => w.IsValidObject() && w.IsVisible())
                .Any())
            {
                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var titleScreen = state.ExistingAll<TitleScreen>()
                .Where(w => w.IsValidObject() && w.IsVisible())
                .FirstOrDefault();

            if (titleScreen != null && titleScreen.PressAnyButton().IsValidObject() && titleScreen.PressAnyButton().IsVisible())
            {
                if (bot.Device.TypeName == XboxDevice.XboxTypeName)
                {
                    yield return new DeviceInputDelegateAction("Progress through title screen", async (gamepad) =>
                    {
                        await bot.Client.PlayerInputKey(bot.PlayerIndex, "Progress through title screen", "Gamepad_FaceButton_Bottom", true).ConfigureAwait(false);
                        await Task.Delay(TimeSpan.FromSeconds(5)).ConfigureAwait(false);
                        await gamepad.PressAndReleaseButtonsAsync(GamepadButtons.A, TimeSpan.FromMilliseconds(150)).ConfigureAwait(false);
                        await Task.Delay(TimeSpan.FromSeconds(2)).ConfigureAwait(false);
                        await gamepad.PressAndReleaseButtonsAsync(GamepadButtons.B, TimeSpan.FromMilliseconds(150)).ConfigureAwait(false);
                    });
                }
                else
                {
                    yield return new PlayerInputAction("Progress through title screen", "Gamepad_FaceButton_Bottom", true);
                }
            }
            else
            {
                var mainMenu = state.ExistingAll<Menu>()
                    .Where(w => w.IsValidObject() && w.IsVisible())
                    .FirstOrDefault();

                if (mainMenu != null && !mainMenu.PromptOnScreen)
                {
                    var gameMode = mainMenu.GameModeIndex;

                    if (gameMode == 0 || mainMenu.OnlineDisabled)
                    {
                        yield return new PlayerInputAction("Start Game", "Gamepad_FaceButton_Bottom", true);
                    }
                    else if (gameMode == 1)
                    {
                        var nextGameMode = (gameMode + 1) % mainMenu.GameModeCount;
                        yield return new PlayerInputAction($"Change Selection to {nextGameMode}", "Gamepad_RightShoulder", false);
                    }
                }
            }
        }
    }
}
