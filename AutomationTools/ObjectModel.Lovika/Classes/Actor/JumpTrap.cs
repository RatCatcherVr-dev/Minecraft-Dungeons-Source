// ------------------------------------------------------------------------------
// <copyright file="JumpTrap.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;

    [GameClass("BP_JumpTrap_C")]
    public class JumpTrap : PropActor
    {
        public JumpTrap(IGameObject obj) : base(obj)
        {
        }
    }
}
