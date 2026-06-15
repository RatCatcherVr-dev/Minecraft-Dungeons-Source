// ------------------------------------------------------------------------------
// <copyright file="TitleScreen.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.UI
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;

    [GameClass("UMG_TitleScreen_C")]
    [GameProperty("PressAnyButton")]
    public class TitleScreen : UserWidget
    {
        public TitleScreen(IGameObject obj) : base(obj)
        {
        }

        public ObjectInfo PressAnyButtonInfo => this.LinkedObjectInfo("PressAnyButton");

        public UserWidget PressAnyButton()
        {
            return this.GameState.Existing<UserWidget>(this.PressAnyButtonInfo);
        }

        public Task<UserWidget> GetPressAnyButton(TimeSpan? maxAge = null)
        {
            return this.GameState.Get<UserWidget>(this.PressAnyButtonInfo, maxAge);
        }
    }
}
