// ------------------------------------------------------------------------------
// <copyright file="LovikaUrge.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// The base urge
    /// </summary>
    [UsesParameter(nameof(Parameters.GameStateMaxAge))]
    public class LovikaUrge : UrgeList<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        public LovikaUrge()
        {
            this.Urges.Add(FuncUrge<LovikaGameState, LovikaBot, LovikaBotActionBase>.Create(this.Name, this.Logger, (state, bot) => this.RunFunctions?.SelectMany(f => f(state, bot))));
        }

        protected virtual IEnumerable<Func<LovikaGameState, LovikaBot, IEnumerable<RecommendedAction>>> RunFunctions => Enumerable.Repeat<Func<LovikaGameState, LovikaBot, IEnumerable<RecommendedAction>>>(this.RunImpl, 1);

        public override async Task<IEnumerable<IRecommendedAction<LovikaBotActionBase>>> Run(LovikaGameState state, LovikaBot bot)
        {
            if (!await this.Update(state, bot).ConfigureAwait(false))
            {
                return Enumerable.Empty<RecommendedAction>();
            }

            return await base.Run(state, bot).ConfigureAwait(false);
        }

        /// <summary>
        /// Runs the urge given the existing game state, returning one or more weighted bot actions.
        /// </summary>
        /// <param name="state">The game state.</param>
        /// <param name="bot">The bot to control.</param>
        /// <returns>An enumeration of weighted bot actions.</returns>
        protected virtual IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            yield break;
        }

        /// <summary>
        /// Updates the game state and determines whether or not the urge should run.
        /// </summary>
        /// <param name="state">The game state.</param>
        /// <param name="bot">The bot to control.</param>
        /// <returns>True if the urge should be run.</returns>
        protected virtual async Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            var gameMaxAge = this.GetParameter<TimeSpan>(bot, Parameters.GameStateMaxAge);

            var loadingScreen = (await state.GetAll<LoadingScreenInitializer>(TimeSpan.Zero).ConfigureAwait(false)).FirstOrDefault();
            if (loadingScreen != null && loadingScreen.IsLoading())
            {
                return false;
            }

            if (!(await state.GetAll<UWorld>(gameMaxAge).ConfigureAwait(false)).Any())
            {
                return false;
            }

            return true;
        }
    }
}
