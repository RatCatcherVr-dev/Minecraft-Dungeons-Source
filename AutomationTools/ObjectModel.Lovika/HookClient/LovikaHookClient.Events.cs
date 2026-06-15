//------------------------------------------------------------------------------
// <copyright file="LovikaHookClient.Events.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using System;
    using Microsoft.GNS.ObjectModel.Lovika.EventArgs;
    using Microsoft.GNS.UnrealToolkit.HookClient;
    using Microsoft.Internal.GamesTest.Rpc.Client;

    /// <summary>
    /// Class containing all game hooks and events.
    /// Derives from the <see cref="UnrealHookClient"/> inheriting all engine events.
    /// Partial class containing Event logic.
    /// </summary>
    public partial class LovikaHookClient
    {
        /// <summary>
        /// Event that signifies a bot has finished its list of actions.
        /// </summary>
        public event EventHandler<GameBotEventArgs> BotAutomationFinished;

        /// <summary>
        /// Event that signifies a bot has temporarily paused executing actions.
        /// </summary>
        public event EventHandler<GameBotEventArgs> BotAutomationPaused;

        /// <summary>
        /// Event that signifies a bot has resumed executing actions.
        /// </summary>
        public event EventHandler<GameBotEventArgs> BotAutomationResumed;

        /// <summary>
        /// Event that signifies a bot has started an action.
        /// </summary>
        public event EventHandler<GameBotActionEventArgs> BotActionStarted;

        /// <summary>
        /// Event that indicates a bot has finished an action.
        /// </summary>
        public event EventHandler<GameBotActionEventArgs> BotActionFinished;

        /// <summary>
        /// Subscribe the distributing event handler for game events.
        /// </summary>
        public override void SubscribeGameEvents()
        {
            this.SubscribeEventHandler("BotAutomationFinished", this.BotAutomationFinishedEventHandler);
            this.SubscribeEventHandler("BotAutomationPaused", this.BotAutomationPausedEventHandler);
            this.SubscribeEventHandler("BotAutomationResumed", this.BotAutomationResumedEventHandler);

            this.SubscribeEventHandler("BotActionStarted", this.BotActionStartedEventHandler);
            this.SubscribeEventHandler("BotActionFinished", this.BotActionFinishedEventHandler);
        }

        private void BotAutomationFinishedEventHandler(object sender, RpcEventArgs eventArgs)
        {
            this.BotAutomationFinished?.Invoke(sender, new GameBotEventArgs(eventArgs.Archive));
        }

        private void BotAutomationPausedEventHandler(object sender, RpcEventArgs eventArgs)
        {
            this.BotAutomationPaused?.Invoke(sender, new GameBotEventArgs(eventArgs.Archive));
        }

        private void BotAutomationResumedEventHandler(object sender, RpcEventArgs eventArgs)
        {
            this.BotAutomationResumed?.Invoke(sender, new GameBotEventArgs(eventArgs.Archive));
        }

        private void BotActionStartedEventHandler(object sender, RpcEventArgs eventArgs)
        {
            this.BotActionStarted?.Invoke(sender, new GameBotActionEventArgs(eventArgs.Archive));
        }

        private void BotActionFinishedEventHandler(object sender, RpcEventArgs eventArgs)
        {
            this.BotActionFinished?.Invoke(sender, new GameBotActionEventArgs(eventArgs.Archive));
        }
    }
}
