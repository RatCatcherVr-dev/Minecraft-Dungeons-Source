// ------------------------------------------------------------------------------
// <copyright file="UWidget.cs" company="Microsoft">
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
    using Microsoft.GNS.GameToolkit;

    [GameClass("Widget")]
    [GameProperty("bIsEnabled", "Visibility", "Slot", "Slot.Parent")]
    public class UWidget : UObject
    {
        public UWidget(IGameObject obj) : base(obj)
        {
        }

        public bool IsEnabled => NumericHelpers.ParseBool(this["bIsEnabled"] as string) ?? false;

        public string Visibility => this["Visibility"] as string;

        public virtual bool IsValidObject()
        {
            return this.Name != "WidgetArchetype";
        }

        public virtual bool IsVisible()
        {
            return this.IsEnabled
                && this.Visibility != "Collapsed"
                && this.Visibility != "Hidden";
        }
    }
}
