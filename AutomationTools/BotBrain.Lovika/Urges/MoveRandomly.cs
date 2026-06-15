// ------------------------------------------------------------------------------
// <copyright file="MoveRandomly.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An urge which moves the bot randomly.
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance), nameof(Parameters.AllowZAxis))]
    public class MoveRandomly : LovikaInGameUrge
    {
        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var distance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var allowZAxis = this.GetParameter<bool>(bot, Parameters.AllowZAxis);

            var player = state.PlayerCharacter(bot.PlayerIndex);
            var playerLocation = player.Location().Value;

            var newLocation = this.GetNewLocation(playerLocation, distance, allowZAxis);
            yield return new MoveToVector3Action($"Move randomly to {newLocation}", newLocation);
        }

        private Vector3 GetNewLocation(Vector3 player, float distance, bool allowZAxis)
        {
            var random = new Random((int)DateTime.UtcNow.Ticks);

            int dimensions = 2 + (allowZAxis ? 1 : 0);
            double scale = distance / Math.Sqrt(dimensions);
            double x = ((random.NextDouble() * 2) - 1) * scale;
            double y = ((random.NextDouble() * 2) - 1) * scale;
            double z = allowZAxis ? Math.Sqrt(Math.Pow(distance, 2) - Math.Pow(x, 2) - Math.Pow(y, 2)) : 0;

            return player + new Vector3((float)x, (float)y, (float)z);
        }
    }
}
