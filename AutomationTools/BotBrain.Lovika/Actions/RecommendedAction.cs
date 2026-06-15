// ------------------------------------------------------------------------------
// <copyright file="RecommendedAction.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;

    public class RecommendedAction : RecommendedAction<LovikaBotActionBase>
    {
        public RecommendedAction(LovikaBotActionBase action) : base(action)
        {
        }

        public RecommendedAction(double urgency, LovikaBotActionBase action) : base(urgency, action)
        {
        }
    }
}
