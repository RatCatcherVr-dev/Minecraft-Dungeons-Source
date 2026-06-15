// ------------------------------------------------------------------------------
// <copyright file="LovikaInGameUrge.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Linq;
    using System.Numerics;
    using System.Reflection;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Microsoft.GNS.UnrealToolkit.HookClient.Exceptions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge that runs in game
    /// </summary>
    [UsesParameter(nameof(Parameters.PlayerMaxAge), nameof(Parameters.GameStateMaxAge))]
    public class LovikaInGameUrge : LovikaUrge
    {
        private ConcurrentDictionary<string, Dictionary<string, bool>> failedAttempt = new ConcurrentDictionary<string, Dictionary<string, bool>>();

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            if (!state.ExistingAll<UWorld>().Where(w => w.Name == "Ingame" || w.Name == "Lobby").Any())
            {
                return false;
            }

            var gameMaxAge = this.GetParameter<TimeSpan>(bot, Parameters.GameStateMaxAge);
            var playerMaxAge = this.GetParameter<TimeSpan>(bot, Parameters.PlayerMaxAge);

            var gameState = (await state.GetAll<DungeonsGameState>(gameMaxAge).ConfigureAwait(false)).SingleOrDefault();
            if (gameState == null
                || gameState.IsCinematicPlaying()
                || gameState.IsGameOver)
            {
                return false;
            }

            var player = await state.GetPlayerController(bot.PlayerIndex, playerMaxAge).ConfigureAwait(false);
            if (player == null
                || player.IsOnVictoryHUD
                || player.PlayerIsWaiting)
            {
                return false;
            }

            var playerController = await player.GetPawn(playerMaxAge).ConfigureAwait(false);
            if (!playerController.IsInWorld())
            {
                return false;
            }

            return true;
        }

        protected bool IsWidgetForActor(ContextSensitiveWidget widget, AActor actor)
        {
            if (widget == null || actor == null)
            {
                return false;
            }

            return widget.InteractableActorInfo.Equals(actor.Info);
        }

        protected bool IsWidgetForLocation(ContextSensitiveWidget widget, Vector3 location)
        {
            if (widget == null)
            {
                return false;
            }

            return widget.Location()?.Equals(location) ?? false;
        }

        protected void ClearFailedAttempts(LovikaBot bot)
        {
            if (this.failedAttempt.ContainsKey(bot.Name))
            {
                this.failedAttempt[bot.Name].Clear();
            }
        }

        protected bool CanAttemptGetAll<T>(LovikaBot bot) where T : class, IGameObject
        {
            var className = this.GetClassName<T>();
            return this.CanAttemptGetAll(bot, className);
        }

        protected bool CanAttemptGetAll(LovikaBot bot, string className)
        {
            var dict = this.GetCurrentFailedAttemptDict(bot);
            return !dict.ContainsKey(className);
        }

        protected async Task<IEnumerable<T>> AttemptGetAll<T>(LovikaGameState state, LovikaBot bot, string className, TimeSpan? maxAge = null) where T : class, IGameObject
        {
            try
            {
                return await state.GetAll<T>(className, maxAge).ConfigureAwait(false);
            }
            catch (ElementNotFoundException)
            {
                var dict = this.GetCurrentFailedAttemptDict(bot);
                dict[className] = true;
            }

            return Enumerable.Empty<T>();
        }

        protected async Task<IEnumerable<T>> AttemptGetAll<T>(LovikaGameState state, LovikaBot bot, TimeSpan? maxAge = null) where T : class, IGameObject
        {
            try
            {
                return await state.GetAll<T>(maxAge).ConfigureAwait(false);
            }
            catch (ElementNotFoundException)
            {
                var dict = this.GetCurrentFailedAttemptDict(bot);
                var className = this.GetClassName<T>();
                dict[className] = true;
            }

            return Enumerable.Empty<T>();
        }

        private string GetClassName<T>() where T : class, IGameObject
        {
            var attribute = typeof(T).GetCustomAttributes(typeof(GameClassAttribute)).FirstOrDefault();
            if (attribute is GameClassAttribute gameClassAttribute)
            {
                return gameClassAttribute.ClassName;
            }

            return nameof(T);
        }

        private Dictionary<string, bool> GetCurrentFailedAttemptDict(LovikaBot bot)
        {
            return this.failedAttempt.GetOrAdd(bot.Name, new Dictionary<string, bool>());
        }
    }
}
