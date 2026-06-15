// ------------------------------------------------------------------------------
// <copyright file="TraverseLevel.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.UnrealToolkit.HookClient.Exceptions;

    /// <summary>
    /// An Urge to walk to the next tile door <see cref="Vector3"/>
    /// </summary>
    public class TraverseLevel : LovikaLevelUrge
    {
        private readonly ConcurrentDictionary<string, Vector3?> tileCache = new ConcurrentDictionary<string, Vector3?>();

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var location = await this.GetNextTileDoor(bot).ConfigureAwait(false);
            this.TryUpdateLastPosition(bot, location);
            return location.HasValue;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            if (this.TryGetPosition(bot, out Vector3? position))
            {
                yield return new MoveToVector3Action($"Move to tile door {position.Value}", position.Value);
            }
        }

        protected virtual bool TryUpdateLastPosition(LovikaBot bot, Vector3? newPosition)
        {
            bool updated = true;
            if (this.tileCache.ContainsKey(bot.Name))
            {
                updated = this.tileCache[bot.Name] != newPosition;
            }

            if (updated)
            {
                this.tileCache[bot.Name] = newPosition;
            }

            return updated;
        }

        protected bool TryGetPosition(LovikaBot bot, out Vector3? position)
        {
            if (this.tileCache.TryGetValue(bot.Name, out position))
            {
                return position.HasValue;
            }

            return false;
        }

        private async Task<Vector3?> GetNextTileDoor(LovikaBot bot)
        {
            try
            {
                return await bot.Client.GetNextTileDoor(bot.PlayerIndex).ConfigureAwait(false);
            }
            catch (HookFailedException e) when (e.Message.Contains("The game-side function \"GetNextTileDoor\" was called and failed."))
            {
                return null;
            }
        }
    }
}
