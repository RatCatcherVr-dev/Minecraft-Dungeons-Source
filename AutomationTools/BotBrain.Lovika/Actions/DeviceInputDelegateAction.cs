// ------------------------------------------------------------------------------
// <copyright file="DeviceInputDelegateAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using DeviceConsole.Environments.Input;
    using DeviceConsole.XboxExtensions;

    public class DeviceInputDelegateAction : BotActionDelegate
    {
        /// <summary>
        /// A class representing bot actions which call a delegate gamepad input function with the bot.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="action">The delegate to invoke.</param>
        public DeviceInputDelegateAction(string name, Func<IControllableGamepad, Task> action = null) : base(name)
        {
            this.GamepadAction = action;
            this.Action = this.ControllerInput;
        }

        public Func<IControllableGamepad, Task> GamepadAction { get; set; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as DeviceInputDelegateAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.GamepadAction == action.GamepadAction;
        }

        private async Task ControllerInput(LovikaBot bot)
        {
            if (this.GamepadAction == null)
            {
                return;
            }

            try
            {
                var gamepad = await this.Connect(bot).ConfigureAwait(false);
                await this.GamepadAction.Invoke(gamepad).ConfigureAwait(false);
            }
            finally
            {
                await this.Disconnect(bot).ConfigureAwait(false);
            }
        }

        private async Task<IControllableGamepad> Connect(LovikaBot bot)
        {
            if (bot.Device.TypeName == ExtendedXboxDevice.XboxTypeName)
            {
                var input = (bot.Device as ExtendedXboxDevice).XdkInput;
                return await input.ConnectDeviceAsync<IControllableGamepad>(default).ConfigureAwait(false);
            }

            throw new Exception($"Device input action does not support {bot.Device.TypeName} devices");
        }

        private async Task Disconnect(LovikaBot bot)
        {
            if (bot.Device.TypeName == ExtendedXboxDevice.XboxTypeName)
            {
                var input = (bot.Device as ExtendedXboxDevice).XdkInput;
                await input.DisconnectAllDevicesAsync(default).ConfigureAwait(false);
            }
        }
    }
}
