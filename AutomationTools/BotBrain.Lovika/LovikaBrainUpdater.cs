// ------------------------------------------------------------------------------
// <copyright file="LovikaBrainUpdater.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Net.Sockets;
    using System.Text;
    using System.Threading;
    using System.Threading.Tasks;
    using DeviceConsole.Infrastructure;
    using DeviceConsole.Win10;
    using Microsoft.GNS.BotBrain.Devices;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Infrastructure.Composition;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;
    using Microsoft.GNS.ObjectModel.Lovika;

    public class LovikaBrainUpdater : BrainUpdaterBase<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        private readonly IDeviceProvider deviceProvider;

        private readonly IDictionary<LovikaHookClient, LovikaGameState> states = new Dictionary<LovikaHookClient, LovikaGameState>();

        private readonly Dictionary<string, LovikaHookClient> hookClients = new Dictionary<string, LovikaHookClient>();

        public LovikaBrainUpdater(IBrainSerializer serializer, IDeviceProvider deviceProvider, ILogger logger = null)
            : base(serializer, logger: logger)
        {
            this.deviceProvider = deviceProvider;
        }

        public override IReadOnlyCollection<LovikaGameState> States => this.states.Values.ToList();

        public override async Task<IAutomatedBot> AddBotAsync(string device, IBotBrain brain = null, string botName = null, int playerIndex = 0, CancellationToken token = default)
        {
            var hookClient = await this.FindOrCreateHookClient(device).ConfigureAwait(false);
            var gameState = this.FindOrCreateGameState(device, hookClient);
            var bot = await this.AddBotAsync(device, gameState, brain, botName, playerIndex, token).ConfigureAwait(false);
            return bot;
        }

        protected override async Task<LovikaBot> CreateBotAsync(string deviceName, LovikaGameState state, string botName, int playerIndex)
        {
            var device = await this.deviceProvider.FromNameAsync(deviceName).ConfigureAwait(false);

            var hookClient = await this.FindOrCreateHookClient(device).ConfigureAwait(false);

            hookClient.SubscribeEngineEvents();
            hookClient.SubscribeGameEvents();

            try
            {
                await hookClient.AddAutomation(playerIndex).ConfigureAwait(false);
                if (device.TypeName == Win10Device.Win10DeviceTypeName)
                {
                    await hookClient.SetGamepadActive(playerIndex, true).ConfigureAwait(false);
                }
            }
            catch (Exception e)
            {
                this.Logger.Error(e.Message);
            }

            return new LovikaBot(hookClient, device, this.Logger.WithPrefix(botName), playerIndex, botName);
        }

        private LovikaGameState FindOrCreateGameState(string deviceName, LovikaHookClient hookClient)
        {
            if (!this.states.TryGetValue(hookClient, out LovikaGameState state))
            {
                var logger = this.Logger.WithPrefix("GameState").WithPrefix(deviceName);
                state = new LovikaGameState(new LovikaObjectRepository(hookClient), logger: logger);
                this.states.Add(hookClient, state);
            }

            return state;
        }

        private async Task<LovikaHookClient> FindOrCreateHookClient(string deviceName)
        {
            var device = await this.deviceProvider.FromNameAsync(deviceName).ConfigureAwait(false);
            return await this.FindOrCreateHookClient(device).ConfigureAwait(false);
        }

        private async Task<LovikaHookClient> FindOrCreateHookClient(IDevice device)
        {
            this.hookClients.TryGetValue(device.Name, out LovikaHookClient hookClient);
            if (hookClient == null)
            {
                var ip = device.IPAddress;
                if (device.TypeName == Win10Device.Win10DeviceTypeName)
                {
                    ip = await this.ResolveNameAsync(device.Name).ConfigureAwait(false);
                }

                hookClient = new LovikaHookClient(ip.ToString());
                this.hookClients.Add(device.Name, hookClient);
            }

            return hookClient;
        }

        private async Task<IPAddress> ResolveNameAsync(string name)
        {
            if (!IPAddress.TryParse(name, out IPAddress connectionIP))
            {
                IPHostEntry hostEntry = await Dns.GetHostEntryAsync(name).ConfigureAwait(false);
                connectionIP = hostEntry.AddressList.FirstOrDefault(a => a.AddressFamily == AddressFamily.InterNetwork);
                if (connectionIP == null)
                {
                    throw new ArgumentException("IP address could not be determined.", nameof(name));
                }
            }

            return connectionIP;
        }
    }
}
