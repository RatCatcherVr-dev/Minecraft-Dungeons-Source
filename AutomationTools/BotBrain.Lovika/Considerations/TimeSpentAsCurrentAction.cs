// ------------------------------------------------------------------------------
// <copyright file="TimeSpentAsCurrentAction.cs" company="Microsoft">
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
    /// A consideration to penalise / promote the current action being continuously recommended, based on how long it has 
    /// been running for
    /// </summary>
    public class TimeSpentAsCurrentAction : ConsiderationBase<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        public const double MaxTimeSpentAsCurrentActionSeconds = 180.0;

        protected override ConsiderationResult EvaluateImpl(
            LovikaGameState state,
            LovikaBot bot,
            IReadOnlyCollection<IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase>> urges,
            in UrgeResult<LovikaGameState, LovikaBot, LovikaBotActionBase> urgeResult)
        {
            TimeSpan timeSinceActionStart = DateTime.Now - bot.BotData.TimeAtCurrentActionStartUtc;

            double timeSinceActionStartSeconds = timeSinceActionStart.TotalSeconds;

            if (timeSinceActionStartSeconds > MaxTimeSpentAsCurrentActionSeconds)
            {
                timeSinceActionStartSeconds = MaxTimeSpentAsCurrentActionSeconds;
            }

            if (urgeResult.Action.Action.Name == bot.CurrentAction?.Name)
            {
                return new ConsiderationResult((float)timeSinceActionStartSeconds, (float)MaxTimeSpentAsCurrentActionSeconds);
            }
            else
            {
                return new ConsiderationResult(0.0f, (float)MaxTimeSpentAsCurrentActionSeconds);
            }
        }
    }
}
