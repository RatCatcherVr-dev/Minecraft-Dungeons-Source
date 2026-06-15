// ------------------------------------------------------------------------------
// <copyright file="GenericTravelDoor.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Actor.Travel
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;

    [GameClass("BP_GenericTravelDoor_C")]
    public class GenericTravelDoor : InstantTravel
    {
        public GenericTravelDoor(IGameObject obj) : base(obj)
        {
        }
    }
}
