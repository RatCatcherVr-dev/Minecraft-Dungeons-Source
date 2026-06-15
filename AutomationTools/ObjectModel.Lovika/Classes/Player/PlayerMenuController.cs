// ------------------------------------------------------------------------------
// <copyright file="PlayerMenuController.cs" company="Microsoft">
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

    [GameClass("MenuPlayerController")]
    public class PlayerMenuController : PlayerControllerBase
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="PlayerMenuController" /> class.
        /// </summary>
        /// <param name="obj">The game object to encapsulate.</param>
        public PlayerMenuController(IGameObject obj) : base(obj)
        {
        }
    }
}
