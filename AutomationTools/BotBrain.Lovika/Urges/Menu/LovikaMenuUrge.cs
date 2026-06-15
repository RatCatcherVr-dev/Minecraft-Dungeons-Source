// ------------------------------------------------------------------------------
// <copyright file="LovikaMenuUrge.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;

    /// <summary>
    /// An Urge that runs in the menu
    /// </summary>
    public class LovikaMenuUrge : LovikaUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if (!state.ExistingAll<UWorld>().Where(w => w.Name == "Menu").Any())
            {
                return false;
            }

            return true;
        }
    }
}
