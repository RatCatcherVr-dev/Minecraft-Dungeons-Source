// ------------------------------------------------------------------------------
// <copyright file="DeviceInputAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading.Tasks;
    using DeviceConsole.Environments.Input;

    public class DeviceInputAction : DeviceInputDelegateAction
    {
        /// <summary>
        /// Presses and releases the specified <see cref="GamepadButtons"/> after the given duration has elapsed.
        /// </summary>
        /// <param name="name">The action name.</param>
        /// <param name="button">The button to press and release.</param>
        /// <param name="duration">The duration to hold.</param>
        public DeviceInputAction(string name, GamepadButtons button, TimeSpan duration) : base(name)
        {
            this.Button = button;
            this.Duration = duration;
            this.GamepadAction = this.GamepadInput;
        }

        [DataMember]
        public GamepadButtons Button { get; set; }

        [DataMember]
        public TimeSpan Duration { get; set; }

        public override bool Equals(LovikaBotActionBase other)
        {
            var action = other as DeviceInputAction;
            return action != null
                && this.ToString() == action.ToString()
                && this.Button == action.Button
                && this.Duration == action.Duration;
        }

        private Task GamepadInput(IControllableGamepad gamepad)
        {
            return gamepad.PressAndReleaseButtonsAsync(this.Button, this.Duration);
        }
    }
}
