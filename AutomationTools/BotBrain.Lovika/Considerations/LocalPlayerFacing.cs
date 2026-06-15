// ------------------------------------------------------------------------------
// <copyright file="LocalPlayerFacing.cs" company="Microsoft">
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

    /// <summary>
    /// A consideration to promote based the on the angle between the player world location
    /// and the location specified in the action.
    /// </summary>
    public class LocalPlayerFacing : ConsiderationBase<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        protected override ConsiderationResult EvaluateImpl(
            LovikaGameState state,
            LovikaBot bot,
            IReadOnlyCollection<IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase>> urges,
            in UrgeResult<LovikaGameState, LovikaBot, LovikaBotActionBase> urgeResult)
        {
            var action = this.GetActionPosisiton(urgeResult.Action.Action);
            if (action.HasValue)
            {
                var playerPawn = state.PlayerCharacter(bot.PlayerIndex);
                var player = playerPawn.Location();
                var rotation = playerPawn.Rotation();

                if (player.HasValue && rotation.HasValue)
                {
                    var toAction = action.Value - player.Value;
                    var toActionNorm = Vector3.Normalize(toAction);
                    var dot = Vector3.Dot(toActionNorm, playerPawn.GetForwardVector());
                    return new ConsiderationResult(dot + 1f, 2.0f);
                }
            }

            return new ConsiderationResult(1.0f, 1.0f);
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
