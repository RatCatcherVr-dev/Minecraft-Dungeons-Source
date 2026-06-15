// ------------------------------------------------------------------------------
// <copyright file="TeleportForwardAfter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges.Level
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Cache;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// A cheat urge to teleport the player forward after a specified time
    /// </summary>
    [UsesParameter(nameof(Parameters.Frequency))]
    public class TeleportForwardAfter : TraverseLevel, IOnMapLoaded
    {
        private readonly TimeElapsedHelper<LovikaBot> timeOnTile = new TimeElapsedHelper<LovikaBot>();

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.TryUpdateLastPosition(bot, null);
            this.timeOnTile.Remove(bot);
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var frequency = this.GetParameter<TimeSpan>(bot, Parameters.Frequency);
            return this.timeOnTile.IsOlderThan(bot, frequency, DateTime.Now);
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            if (this.TryGetPosition(bot, out Vector3? _))
            {
                yield return new ConsoleCommandAction("Teleport Forward", "Dungeons.Player.TeleportForward")
                {
                    IsWorkaround = true
                };
            }
        }

        protected override bool TryUpdateLastPosition(LovikaBot bot, Vector3? newPosition)
        {
            var updated = base.TryUpdateLastPosition(bot, newPosition);
            if (updated)
            {
                this.timeOnTile.Refresh(bot);
            }

            return updated;
        }
    }
}
