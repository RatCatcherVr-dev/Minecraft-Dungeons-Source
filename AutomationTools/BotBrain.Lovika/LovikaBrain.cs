// ------------------------------------------------------------------------------
// <copyright file="LovikaBrain.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika
{
    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Reflection;
    using System.Runtime.InteropServices;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.BotBrain.Lovika.Urges;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Player;
    using Microsoft.GNS.UnrealToolkit.HookClient.Events;
    using Microsoft.Internal.GamesTest.Rpc.Client;

    public class LovikaBrain : CompoundBrain<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        /// <inheritdoc />
        public override async Task<IEnumerable<IRecommendedAction<LovikaBotActionBase>>> Run(LovikaGameState state, LovikaBot bot)
        {
            var recomendations = await base.Run(state, bot).ConfigureAwait(false);
            var recomendation = recomendations.SingleOrDefault();
            if (recomendation != null)
            {
                var action = recomendation.Action;
                if (action.IsWorkaround)
                {
                    foreach (var urge in this.Urges.OfType<WorkaroundTracker>())
                    {
#pragma warning disable CS4014 // Don't await here as botbrain will contiune while the video capture is being recorded
                        urge.AddWorkaround(state, bot, action);
#pragma warning restore CS4014
                    }
                }
            }

            return recomendations;
        }

        /// <inheritdoc />
        protected override void CreateBotData(LovikaBot bot, IReadOnlyCollection<LovikaGameState> states, CancellationToken token)
        {
            bot.BotData = new LovikaBrainBotData(this.Name, bot, states, token);
            this.CreateBotData(bot, bot.BotData);
        }

        /// <inheritdoc />
        protected override void Start(LovikaBot bot, IReadOnlyCollection<LovikaGameState> states, CancellationToken token)
        {
            // Assign the parameters to the bot, for the urges to use.
            if (bot != null)
            {
                bot.Parameters = this.Parameters;
            }

            base.Start(bot, states, token);
        }

        protected override Task<bool> ExecuteUpdate(LovikaGameState state, LovikaBot bot)
        {
            if (!bot.IsPaused)
            {
                return base.ExecuteUpdate(state, bot);
            }

            return Task.FromResult(false);
        }

        /// <summary>
        /// Handles an urge exception.
        /// </summary>
        /// <param name="urge">The urge that threw the exception.</param>
        /// <param name="bot">The corresponding bot.</param>
        /// <param name="e">The exception.</param>
        protected override void HandleUrgeException(IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase> urge, LovikaBot bot, Exception e)
        {
            if (e is RpcRemoteMethodNotFoundException)
            {
                this.Stop(bot);
            }

            base.HandleUrgeException(urge, bot, e);
        }

        protected override void PreUpdateBot(LovikaGameState state, LovikaBot bot)
        {
            base.PreUpdateBot(state, bot);

            var onMapLoaded = bot.RecentEvents.OfType<MapLoadedEventArgs>().Any();
            foreach (var urge in this.Urges)
            {
                if (onMapLoaded)
                {
                    (urge as IOnMapLoaded)?.OnMapLoaded(state, bot);
                }
            }

            if (onMapLoaded)
            {
                state.ClearAll<PlayerPawn>();
                state.ClearAll<PlayerController>();
            }
        }

        protected override void PostUpdateBot(LovikaGameState state, LovikaBot bot, LovikaBotActionBase previous, LovikaBotActionBase next)
        {
            base.PostUpdateBot(state, bot, previous, next);
            bot.ClearEvents();
        }
    }
}
