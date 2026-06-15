// ------------------------------------------------------------------------------
// <copyright file="MissionSelectedInspector.cs" company="Microsoft">
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

    [GameClass("UMG_MissionSelectedInspector_C")]
    [GameProperty("Locked", "ThreatLevelLocked", "ThreatLevelTooLow", "ThreatLevelTooHigh", "readingStory")]
    public class MissionSelectedInspector : SelectedInspectorBase
    {
        public MissionSelectedInspector(IGameObject obj) : base(obj)
        {
        }

        public bool Locked => NumericHelpers.ParseBool(this["Locked"] as string) ?? false;

        public bool ThreatLevelLocked => NumericHelpers.ParseBool(this["ThreatLevelLocked"] as string) ?? false;

        public bool ThreatLevelTooLow => NumericHelpers.ParseBool(this["ThreatLevelTooLow"] as string) ?? false;

        public bool ThreatLevelTooHigh => NumericHelpers.ParseBool(this["ThreatLevelTooHigh"] as string) ?? false;

        public bool ReadingStory => NumericHelpers.ParseBool(this["readingStory"] as string) ?? false;

        public bool CanStartMission()
        {
            return !(this.IsRequesting
                || this.Locked
                || this.ThreatLevelLocked
                || this.ThreatLevelTooLow
                || this.ThreatLevelTooHigh
                || this.ReadingStory);
        }
    }
}
