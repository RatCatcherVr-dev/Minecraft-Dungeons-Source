// ------------------------------------------------------------------------------
// <copyright file="MissionSelectMap.cs" company="Microsoft">
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

    [GameClass("UMG_MissionSelectMapWidget_C")]
    [GameProperty("PlayerController", "SelectedLevelName")]
    public class MissionSelectMap : UserWidget
    {
        public MissionSelectMap(IGameObject obj) : base(obj)
        {
        }

        public ObjectInfo PlayerControllerInfo => this.LinkedObjectInfo("PlayerController");

        public string SelectedLevelName => this["SelectedLevelName"] as string;

        public bool HasSelectedMission()
        {
            return this.SelectedLevelName != "Invalid";
        }
    }
}
