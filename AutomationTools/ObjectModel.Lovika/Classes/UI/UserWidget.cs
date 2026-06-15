// ------------------------------------------------------------------------------
// <copyright file="UserWidget.cs" company="Microsoft">
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

    [GameClass("UserWidget")]
    public class UserWidget : UWidget
    {
        public UserWidget(IGameObject obj) : base(obj)
        {
        }
    }
}
