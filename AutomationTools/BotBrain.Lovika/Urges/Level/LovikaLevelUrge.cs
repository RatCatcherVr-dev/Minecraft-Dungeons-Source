// ------------------------------------------------------------------------------
// <copyright file="LovikaLevelUrge.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge that runs on a mission
    /// </summary>
    [UsesParameter(nameof(Parameters.GameStateMaxAge))]
    public class LovikaLevelUrge : LovikaInGameUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if (!state.ExistingAll<UWorld>().Where(w => w.Name == "Ingame").Any())
            {
                return false;
            }

            TimeSpan gameMaxAge = this.GetParameter<TimeSpan>(bot, Parameters.GameStateMaxAge);
            var gameBp = await state.GetAll<GameBP>("BP_GameBP_C", gameMaxAge).ConfigureAwait(false);
            if (!gameBp.Any())
            {
                return false;
            }

            return true;
        }
    }
}
