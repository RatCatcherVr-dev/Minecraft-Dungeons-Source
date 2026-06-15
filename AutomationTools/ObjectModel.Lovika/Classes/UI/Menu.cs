// ------------------------------------------------------------------------------
// <copyright file="Menu.cs" company="Microsoft">
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
    using Microsoft.GNS.GameToolkit;

    [GameClass("UMG_Menu_C")]
    [GameProperty("GameModeIndex", "GameModeCount", "OnlineDisabled", "PromptOnScreen")]
    public class Menu : UserWidget
    {
        public Menu(IGameObject obj) : base(obj)
        {
        }

        public int GameModeIndex => NumericHelpers.ParseInt(this["GameModeIndex"] as string) ?? 1;

        public int GameModeCount => NumericHelpers.ParseInt(this["GameModeCount"] as string) ?? 0;

        public bool OnlineDisabled => NumericHelpers.ParseBool(this["OnlineDisabled"] as string) ?? false;

        public bool PromptOnScreen => NumericHelpers.ParseBool(this["PromptOnScreen"] as string) ?? false;
    }
}
