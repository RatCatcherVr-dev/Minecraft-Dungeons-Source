// ------------------------------------------------------------------------------
// <copyright file="WorkaroundTracker.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.IO;
    using System.Linq;
    using System.Numerics;
    using System.Reflection;
    using System.Text;
    using System.Threading.Tasks;
    using DeviceConsole.Environments.Capture;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Newtonsoft.Json;

    /// <summary>
    /// An Urge which tracks when workarounds are used and stores them in a file
    /// </summary>
    public class WorkaroundTracker : LovikaUrge, IOnMapLoaded
    {
        private readonly ConcurrentDictionary<string, WorkaroundData> data = new ConcurrentDictionary<string, WorkaroundData>();

        [JsonProperty]
        public string SaveDirectory { get; set; } = null;

        [JsonProperty]
        public bool LogNoWorkarounds { get; set; } = false;

        [JsonProperty]
        public bool CaptureWorkaround { get; set; } = true;

        private string CurrentSaveDirectory => this.SaveDirectory ?? Path.GetDirectoryName(Assembly.GetEntryAssembly().Location);

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            var data = this.GetData(bot);
            this.SaveData(bot, data);
            data.History.Clear();
        }

        public async Task AddWorkaround(LovikaGameState state, LovikaBot bot, LovikaBotActionBase currentAction)
        {
            var data = this.GetData(bot);
            var playerLocation = state.PlayerCharacter(bot.PlayerIndex)?.Location();

            var workaround = new WorkaroundActionData
            {
                Action = currentAction,
                Time = DateTime.UtcNow,
                Location = playerLocation
            };

            data.History.Add(workaround);

            if (this.CaptureWorkaround)
            {
                await this.CaptureVideo(bot, TimeSpan.FromSeconds(30), workaround).ConfigureAwait(false);
            }
        }

        private async Task<DvrMetadata> CaptureVideo(LovikaBot bot, TimeSpan duration, WorkaroundActionData workaround)
        {
            var fileNameLocation = this.GetWorkaroundVideoLocation(bot, workaround);

            var capture = await bot.CaptureVideo(duration, fileNameLocation).ConfigureAwait(false);
            this.Logger.Info($"Saved capture to {capture.Location}, had a length of {capture.ClipLength}");
            return capture;
        }

        private string GetWorkaroundVideoLocation(LovikaBot bot, WorkaroundActionData workaround)
        {
            var fileName = $"{bot.Name.Replace(':', '-')}-{workaround.Time:ddMMyyyy-HH:mm:ss}-{bot.BotData.MapName}.mp4";
            return Path.Combine(this.CurrentSaveDirectory, fileName);
        }

        private void SaveData(LovikaBot bot, WorkaroundData data)
        {
            var history = data.History;
            if (!this.LogNoWorkarounds && history.Count == 0)
            {
                return;
            }

            var fileLocation = this.GetWorkaroundFileLocation(bot);
            this.Logger.Debug($"Saving workarounds to file '{fileLocation}'");

            using (var file = new StreamWriter(fileLocation, true))
            {
                file.WriteLine($"Map: {bot.BotData.MapName}. Seed: {bot.BotData.MapSeed}");
                file.WriteLine($"Workaround(s) used: {history.Count}");
                history.ForEach(action => file.WriteLine(action.ToString()));
                file.WriteLine();
            }
        }

        private string GetWorkaroundFileLocation(LovikaBot bot)
        {
            var fileName = bot.Name.Replace(':', '-') + "-workarounds.txt";
            return Path.Combine(this.CurrentSaveDirectory, fileName);
        }

        private WorkaroundData GetData(LovikaBot bot) => this.data.GetOrAdd(bot.Name, new WorkaroundData());

        private class WorkaroundData
        {
            public List<WorkaroundActionData> History { get; private set; } = new List<WorkaroundActionData>();
        }

        private class WorkaroundActionData
        {
            public LovikaBotActionBase Action { get; set; }

            public DateTime Time { get; set; }

            public Vector3? Location { get; set; }

            public override string ToString()
            {
                var time = this.Time.ToString("s", System.Globalization.CultureInfo.InvariantCulture);
                var typeName = this.Action.GetType().Name;
                var actionName = this.Action.Name;
                var location = this.Location.HasValue ? $"@{this.Location.Value}" : string.Empty;

                return $"[{time}] [{typeName}] {actionName} {location}";
            }
        }
    }
}
