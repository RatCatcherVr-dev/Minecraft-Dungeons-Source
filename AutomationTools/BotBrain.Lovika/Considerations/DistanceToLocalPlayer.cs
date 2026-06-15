// ------------------------------------------------------------------------------
// <copyright file="DistanceToLocalPlayer.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Considerations
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Reflection;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Infrastructure.Considerations;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// A consideration to promote based on the distance between the player world location
    /// and the location specified in the action.
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance))]
    public class DistanceToLocalPlayer : ConsiderationBase<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        public const float DefaultMaxDistance = 10000.0f;

        protected override ConsiderationResult EvaluateImpl(
            LovikaGameState state,
            LovikaBot bot,
            IReadOnlyCollection<IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase>> urges,
            in UrgeResult<LovikaGameState, LovikaBot, LovikaBotActionBase> urgeResult)
        {
            var action = this.GetActionPosisiton(urgeResult.Action.Action);
            if (action.HasValue)
            {
                var maxDistance = this.GetMaxDistance(bot, state, urgeResult.Urge);

                var playerPawn = state.PlayerCharacter(bot.PlayerIndex);
                var player = playerPawn.Location();

                if (player.HasValue)
                {
                    float distanceToPlayer = Math.Min(Vector3.Distance(action.Value, player.Value), maxDistance);
                    return new ConsiderationResult(distanceToPlayer, maxDistance);
                }
            }

            return new ConsiderationResult(0.0f, 1.0f);
        }

        private float GetMaxDistance(LovikaBot bot, LovikaGameState state, IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase> urge)
        {
            var names = this.GetParameterNames(Parameters.MaxDistance, state, urge);
            return bot.Parameters.TryGet<float>(names, out float value) ? value : DefaultMaxDistance;
        }

        private Vector3? GetActionPosisiton(LovikaBotActionBase action)
        {
            var worldPosAttribute = action.GetType().GetCustomAttribute(typeof(ActionWithWorldPositionAttribute)) as ActionWithWorldPositionAttribute;
            if (worldPosAttribute == null)
            {
                return null;
            }

            return worldPosAttribute.GetPosition(action, this.Logger);
        }
    }
}
