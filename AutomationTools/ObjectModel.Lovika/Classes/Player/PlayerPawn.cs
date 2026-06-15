// ------------------------------------------------------------------------------
// <copyright file="PlayerPawn.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.Player
{
    using System;
    using System.Collections.Generic;
    using System.Globalization;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Text.RegularExpressions;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameToolkit;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;

    [GameClass("PlayerCharacter")]
    [GameProperty("bInInventory", "bHasBegunPlayOnControllingClient", "bHasGeneratedLevelOnControllingClient")]
    public class PlayerPawn : BaseCharacter
    {
        public PlayerPawn(IGameObject obj) : base(obj)
        {
        }

        // These are private fields currently (so they are always false, may need to talk to mojang to see if there are alternatives)
        public bool InInventory => NumericHelpers.ParseBool(this["bInInventory"] as string) ?? false;
        public bool HasBegunPlayOnControllingClient => NumericHelpers.ParseBool(this["bHasBegunPlayOnControllingClient"] as string) ?? false;
        public bool HasGeneratedLevelOnControllingClient => NumericHelpers.ParseBool(this["bHasGeneratedLevelOnControllingClient"] as string) ?? false;
    }
}
