// ------------------------------------------------------------------------------
// <copyright file="RandomNoise.cs" company="Microsoft">
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
    /// A consideration to randomly penalise. Use with Significance to add randomisation to choices
    /// which would otherwise be deterministic
    /// </summary>
    public class RandomNoise : ConsiderationBase<LovikaGameState, LovikaBot, LovikaBotActionBase>
    {
        private readonly Random random = new Random();

        protected override ConsiderationResult EvaluateImpl(
            LovikaGameState state,
            LovikaBot bot,
            IReadOnlyCollection<IUrge<LovikaGameState, LovikaBot, LovikaBotActionBase>> urges,
            in UrgeResult<LovikaGameState, LovikaBot, LovikaBotActionBase> urgeResult)
        {
            return new ConsiderationResult((float)this.random.NextDouble(), 1.0f);
        }
    }
}
