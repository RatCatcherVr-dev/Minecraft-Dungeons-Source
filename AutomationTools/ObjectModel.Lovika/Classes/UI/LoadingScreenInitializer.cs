// ------------------------------------------------------------------------------
// <copyright file="LoadingScreenInitializer.cs" company="Microsoft">
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
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    [GameClass("LoadingScreenInitializer")]
    [GameProperty("LoadingState")]
    public class LoadingScreenInitializer : UObject
    {
        public LoadingScreenInitializer(IGameObject obj) : base(obj)
        {
        }

        public LoadingState LoadingState => (LoadingState)Enum.Parse(typeof(LoadingState), this["LoadingState"] as string, true);

        public bool IsLoading()
        {
            return this.LoadingState != LoadingState.Idle;
        }
    }
}
