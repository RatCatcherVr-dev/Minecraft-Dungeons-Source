// ------------------------------------------------------------------------------
// <copyright file="LobbyBp.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Info
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameToolkit;

    [GameClass("LobbyBP")]
    [GameProperty("AnnouncementsShowed", "AllStartWidgetsAreClosed")]
    public class LobbyBp : GameBP
    {
        public LobbyBp(IGameObject obj) : base(obj)
        {
        }

        public bool AnnouncementsShowed => NumericHelpers.ParseBool(this["AnnouncementsShowed"] as string) ?? false;

        public bool AllStartWidgetsAreClosed => NumericHelpers.ParseBool(this["AllStartWidgetsAreClosed"] as string) ?? false;
    }
}
