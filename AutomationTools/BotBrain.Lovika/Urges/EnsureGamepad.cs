// ------------------------------------------------------------------------------
// <copyright file="EnsureGamepad.cs" company="Microsoft">
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
    using DeviceConsole.Win10;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;

    /// <summary>
    /// An urge to insure the player is using a gamepad.
    /// </summary>
    public class EnsureGamepad : LovikaInGameUrge
    {
        protected override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (bot.Device.TypeName != Win10Device.Win10DeviceTypeName)
            {
                return Task.FromResult(false);
            }

            return base.Update(state, bot);
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var player = state.PlayerController(bot.PlayerIndex);
            if (player != null && !player.GamepadActive)
            {
                yield return new SetGamepadActiveAction($"Force bot into gamepad input", true);
            }
        }
    }
}
