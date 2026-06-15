// ------------------------------------------------------------------------------
// <copyright file="LovikaBrainBotData.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.BotBrain.Lovika.Urges;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.UnrealToolkit.HookClient.Events;
    using Microsoft.Internal.GamesTest.Rpc.Client;

    public class LovikaBrainBotData : BrainBotData<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        public LovikaBrainBotData(string brainName, LovikaBot bot, IReadOnlyCollection<LovikaGameState> states, CancellationToken token) : base(bot, states, token)
        {
            this.BrainName = brainName;
            bot.ActionStarted += (sender, e) => this.TimeAtCurrentActionStartUtc = DateTime.UtcNow;
        }

        public string BrainName { get; private set; }

        public DateTime TimeAtCurrentActionStartUtc { get; private set; } = DateTime.UtcNow;

        // number of times a particular action has been run
        public IDictionary<string, int> ActionRuns { get; set; } = new ConcurrentDictionary<string, int>();

        // number of times an action generated from a particular urge has been run
        public IDictionary<string, int> UrgeRuns { get; set; } = new ConcurrentDictionary<string, int>();

        // number of times a particular action has been seen as the last action before a reboot occurred
        public IDictionary<string, int> FailedRunsByActionName { get; set; } = new ConcurrentDictionary<string, int>();

        public int MaxRecentActions => 20;
        public ConcurrentQueue<LovikaBotActionBase> RecentActionHistory { get; set; } = new ConcurrentQueue<LovikaBotActionBase>();

        /// <inheritdoc />
        public override void PreUpdate(LovikaGameState state, LovikaBot bot)
        {
            base.PreUpdate(state, bot);

            if (bot.RecentEvents.OfType<MapLoadedEventArgs>().Any())
            {
                this.MapName = null;
                this.MapSeed = null;
            }
            else
            {
                if (this.MapName == null || this.MapSeed == null)
                {
                    try
                    {
                        if (bot.CurrentUrge is LovikaInGameUrge)
                        {
                            bot.BotData.MapName = bot.Client.InvokeHookMethod<string>("MapName", "GetMapName", new RpcArchive()).Result;
                            bot.BotData.MapSeed = bot.Client.InvokeHookMethod<string>("Seed", "GetMapSeed", new RpcArchive()).Result;
                        }
                    }
                    catch (Exception e)
                    {
                        bot.Logger.Error("Error collection map name and seed");
                    }
                }
            }
        }

        /// <inheritdoc />
        public override void PostUpdate(LovikaGameState state, LovikaBot bot, IReadOnlyCollection<WeightedUrgeResult<LovikaGameState, LovikaBot, LovikaBotActionBase>> recommendations, IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase> bestUrge, LovikaBotActionBase bestAction)
        {
            base.PostUpdate(state, bot, recommendations, bestUrge, bestAction);

            if (bestUrge?.Equals(bot.CurrentUrge) == false)
            {
                this.UrgeRuns.Increment(bestUrge.ToString(), 1);
            }

            if (bestAction?.Equals(bot.CurrentAction) == false)
            {
                this.ActionRuns.Increment(bestAction.ToString(), 1);
                this.RecentActionHistory.Enqueue(bestAction);
                while (this.RecentActionHistory.Count > this.MaxRecentActions)
                {
                    this.RecentActionHistory.TryDequeue(out _);
                }
            }
        }

        public string MapName { get; set; }

        public string MapSeed { get; set; }
    }
}
