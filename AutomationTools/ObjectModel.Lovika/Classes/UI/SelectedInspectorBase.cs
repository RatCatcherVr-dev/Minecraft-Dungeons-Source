// ------------------------------------------------------------------------------
// <copyright file="SelectedInspectorBase.cs" company="Microsoft">
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

    [GameClass("UMG_SelectedInspectorBase_C")]
    [GameProperty("IsOpen", "IsRequesting")]
    public class SelectedInspectorBase : UserWidget
    {
        public SelectedInspectorBase(IGameObject obj) : base(obj)
        {
        }

        public bool IsOpen => NumericHelpers.ParseBool(this["IsOpen"] as string) ?? false;

        public bool IsRequesting => NumericHelpers.ParseBool(this["IsRequesting"] as string) ?? false;
    }
}
