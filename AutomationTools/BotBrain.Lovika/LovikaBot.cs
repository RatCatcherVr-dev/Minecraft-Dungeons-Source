// ------------------------------------------------------------------------------
// <copyright file="LovikaBot.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Reflection;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using DeviceConsole.Environments.Capture;
    using DeviceConsole.Infrastructure;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Events;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.EventArgs;
    using Microsoft.GNS.UnrealToolkit.HookClient.Events;

    [System.Diagnostics.DebuggerDisplay("LovikaBot {Name, nq}")]
    public partial class LovikaBot : DelayedEventAggregator<EventArgs>, IAutomatedBot<LovikaBotActionBase>, IBotWithEvents<LovikaBotActionBase>, IEventProcessor
    {
        private LovikaHookClient client;

        public LovikaBot(LovikaHookClient client, IDevice device = null, ILogger logger = null, int playerIndex = 0, string name = null)
        {
            this.client = client;
            this.Device = device;
            this.Logger = logger ?? new NullLogger();
            this.PlayerIndex = playerIndex;
            this.Name = name ?? $"Bot {this.PlayerIndex}";
            this.RegisterEvents();
        }

        public event EventHandler<BotActionEventArgs> ActionStarted;

        public event EventHandler<BotActionFailedEventArgs> ActionFailed;

        public event EventHandler<BotActionEventArgs> ActionFinished;

        public IDevice Device { get; }

        public ILogger Logger { get; }

        public int PlayerIndex { get; }

        public string Name { get; set; }

        public LovikaBotActionBase CurrentAction { get; set; }

        object IAutomatedBot.CurrentAction => this.CurrentAction;

        public object CurrentUrge { get; set; }

        public IParameterManager Parameters { get; set; }

        public bool IsRunning => this.CurrentAction != null;

        public bool IsPaused { get; private set; }

        public LovikaBrainBotData BotData { get; set; }

        public LovikaHookClient Client {
            get => this.client;
            set {
                if (value != null)
                {
                    this.UnregisterEvents(this.Client);
                    this.client = value;
                    this.RegisterEvents();
                }
            }
        }

        public Task Start()
        {
            if (this.CurrentAction != null)
            {
                if (this.CurrentAction.GetType().GetCustomAttribute(typeof(UninterruptibleActionAttribute)) is UninterruptibleActionAttribute uninterruptibleAttribute)
                {
                    using (var tokenSource = new CancellationTokenSource(uninterruptibleAttribute.Timeout))
                    {
                        return this.WaitForActionFinished(() => this.CurrentAction.Run(this), tokenSource.Token);
                    }
                }
                else
                {
                    return this.CurrentAction.Run(this);
                }
            }

            return Task.CompletedTask;
        }

        public Task Stop()
        {
            return Task.Run(
                () =>
                {
                    this.CurrentAction = null;
                });
        }

        public async Task Reset()
        {
            await this.Stop().ConfigureAwait(false);
            this.CurrentAction = default;
        }

        private void RegisterEvents()
        {
            if (this.client == null)
            {
                return;
            }

            this.client.MapLoadedEvent += this.MapLoadedEventHandler;

            this.client.BotAutomationFinished += this.BotAutomationFinishedEventHandler;
            this.client.BotAutomationPaused += this.BotAutomationPausedEventHandler;
            this.client.BotAutomationResumed += this.BotAutomationResumedEventHandler;

            this.client.BotActionStarted += this.BotActionStartedEventHandler;
            this.client.BotActionFinished += this.BotActionFinishedEventHandler;
        }

        private void UnregisterEvents(LovikaHookClient client)
        {
            client.MapLoadedEvent -= this.MapLoadedEventHandler;

            client.BotAutomationFinished -= this.BotAutomationFinishedEventHandler;
            client.BotAutomationPaused -= this.BotAutomationPausedEventHandler;
            client.BotAutomationResumed -= this.BotAutomationResumedEventHandler;

            client.BotActionStarted -= this.BotActionStartedEventHandler;
            client.BotActionFinished -= this.BotActionFinishedEventHandler;
        }

        private void MapLoadedEventHandler(object sender, MapLoadedEventArgs e)
        {
            this.Publish(this, e);
        }

        private void BotAutomationFinishedEventHandler(object sender, GameBotEventArgs e)
        {
            if (e.PlayerIndex == this.PlayerIndex)
            {
                this.IsPaused = false;
                this.CurrentAction = null;
                this.Publish(this, e);
            }
        }

        private void BotAutomationPausedEventHandler(object sender, GameBotEventArgs e)
        {
            if (e.PlayerIndex == this.PlayerIndex)
            {
                this.IsPaused = true;
                this.Logger.Debug("Bot has been paused");
                this.Publish(this, e);
            }
        }

        private void BotAutomationResumedEventHandler(object sender, GameBotEventArgs e)
        {
            if (e.PlayerIndex == this.PlayerIndex)
            {
                this.IsPaused = false;
                this.Logger.Debug("Bot has resumed");
                this.Publish(this, e);
            }
        }

        private void BotActionStartedEventHandler(object sender, GameBotActionEventArgs e)
        {
            if (e.PlayerIndex == this.PlayerIndex)
            {
                if (this.CurrentAction != null)
                {
                    this.CurrentAction.Status = e.ActionStatus;
                }

                this.OnActionStarted();
            }
        }

        private void BotActionFinishedEventHandler(object sender, GameBotActionEventArgs e)
        {
            if (e.PlayerIndex == this.PlayerIndex)
            {
                if (this.CurrentAction != null)
                {
                    this.CurrentAction.Status = e.ActionStatus;
                }

                this.OnActionFinished(this.CurrentAction);
                this.CurrentAction = null;
            }
        }

        private void OnActionStarted()
        {
            var e = new BotActionStartedEventArgs(this.CurrentAction);
            this.Publish(this, e);
            this.ActionStarted?.Invoke(this, e);
        }

        private void OnActionFinished(LovikaBotActionBase action)
        {
            var e = new BotActionFinishedEventArgs(action);
            this.Publish(this, e);
            this.ActionFinished?.Invoke(this, e);
        }

        private void OnActionFailed(LovikaBotActionBase action, Exception ex)
        {
            var e = new BotActionFailedEventArgs(action, ex);
            this.Publish(this, e);
            this.ActionFailed?.Invoke(this, e);
        }

        public Task<DvrMetadata> CaptureVideo(TimeSpan duration, string destination)
        {
            var videoCapture = this.Device.GetVideoCaptureEnvironment();
            if (videoCapture == null)
            {
                return Task.FromResult((DvrMetadata)null);
            }

            return videoCapture.CaptureVideoAsync(duration, destination);
        }
    }
}
