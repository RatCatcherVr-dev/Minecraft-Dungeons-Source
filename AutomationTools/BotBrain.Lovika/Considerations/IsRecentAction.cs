// ------------------------------------------------------------------------------
// <copyright file="IsRecentAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Considerations
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Infrastructure.Considerations;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;

    /// <summary>
    /// A consideration to promote based on whether this action has recently run
    /// to help prevent cyclical locks.
    /// </summary>
    public class IsRecentAction : ConsiderationBase<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        protected override ConsiderationResult EvaluateImpl(
            LovikaGameState state,
            LovikaBot bot,
            IReadOnlyCollection<IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase>> urges,
            in UrgeResult<LovikaGameState, LovikaBot, LovikaBotActionBase> urgeResult)
        {
            var name = urgeResult.Action.Action.Name;
            var result = 0.0f;

            // ignore current action to prevent oscillation
            if (bot.CurrentAction?.Name != name)
            {
                var recentAttempts = bot.BotData.RecentActionHistory.ToArray().Count(i => i.Name == name);
                if (recentAttempts > 0)
                {
                    var maxAttempts = 5.0f;
                    result = Math.Min(1.0f, recentAttempts / maxAttempts);
                }
            }

            return new ConsiderationResult(result, 1.0f);
        }
    }
}
