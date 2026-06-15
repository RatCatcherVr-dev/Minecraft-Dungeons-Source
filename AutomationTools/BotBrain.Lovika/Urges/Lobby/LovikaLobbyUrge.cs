// ------------------------------------------------------------------------------
// <copyright file="LovikaLobbyUrge.cs" company="Microsoft">
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
    /// An Urge that runs in the lobby
    /// </summary>
    [UsesParameter(nameof(Parameters.GameStateMaxAge))]
    public class LovikaLobbyUrge : LovikaInGameUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if (!state.ExistingAll<UWorld>().Where(w => w.Name == "Lobby").Any())
            {
                return false;
            }

            var gameMaxAge = this.GetParameter<TimeSpan>(bot, Parameters.GameStateMaxAge);
            var lobbyBp = await state.GetAll<LobbyBp>(gameMaxAge).ConfigureAwait(false);
            if (!lobbyBp.Any())
            {
                return false;
            }

            return true;
        }
    }
}
