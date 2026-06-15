// ------------------------------------------------------------------------------
// <copyright file="PlayerController.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Player
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameToolkit;

    [GameClass("BP_PlayerController_C")]
    [GameProperty("bIsChatWheelVisible", "bIsUIInputAllowed", "IsOnVictoryHUD")]
    public class PlayerController : PlayerControllerBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PlayerController" /> class.
        /// </summary>
        /// <param name="obj">The game object to encapsulate.</param>
        public PlayerController(IGameObject obj) : base(obj)
        {
        }

        public bool IsChatWheelVisible => NumericHelpers.ParseBool(this["bIsChatWheelVisible"] as string) ?? true;

        public bool UIInputAllowed => NumericHelpers.ParseBool(this["bIsUIInputAllowed"] as string) ?? true;

        public bool IsOnVictoryHUD => NumericHelpers.ParseBool(this["IsOnVictoryHUD"] as string) ?? false;
    }
}
