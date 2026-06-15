// ------------------------------------------------------------------------------
// <copyright file="PlayerIdle.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An urge which moves the bot randomly when idle.
    /// </summary>
    [UsesParameter(nameof(Parameters.IdleDistanceThreshold), nameof(Parameters.IdleWaitTime))]
    public class PlayerIdle : MoveRandomly
    {
        private readonly ConcurrentDictionary<string, LastPosition> lastPositions = new ConcurrentDictionary<string, LastPosition>();

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var player = state.PlayerCharacter(bot.PlayerIndex);
            var lastPosition = this.lastPositions.GetOrAdd(bot.Name, new LastPosition());

            TimeSpan timeWaitingNotMoving = this.GetParameter<TimeSpan>(bot, Parameters.IdleWaitTime);
            if (DateTime.UtcNow - lastPosition.Time < timeWaitingNotMoving)
            {
                return false;
            }

            float distanceThreshold = this.GetParameter<float>(bot, Parameters.IdleDistanceThreshold);
            if (!player.IsInRangeOf(lastPosition.Position, distanceThreshold))
            {
                lastPosition.Position = player.Location().Value;
                lastPosition.Time = DateTime.UtcNow;
                return false;
            }

            lastPosition.Time = DateTime.UtcNow;
            return true;
        }

        private class LastPosition
        {
            public Vector3 Position { get; set; }
            public DateTime Time { get; set; }
        }
    }
}
