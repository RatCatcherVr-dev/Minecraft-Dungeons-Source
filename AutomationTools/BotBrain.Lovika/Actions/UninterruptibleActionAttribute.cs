// ------------------------------------------------------------------------------
// <copyright file="UninterruptibleActionAttribute.cs" company="Microsoft">
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

    [AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
    public class UninterruptibleActionAttribute : Attribute
    {
        /// <summary>
        /// Mark an action as uninterruptible.
        /// </summary>
        /// <param name="timeout">The timeout.</param>
        public UninterruptibleActionAttribute(TimeSpan timeout)
        {
            this.Timeout = timeout;
        }

        /// <summary>
        /// Mark an action as uninterruptible.
        /// </summary>
        /// <param name="timeout">The timeout in milliseconds.</param>
        public UninterruptibleActionAttribute(double timeout)
        {
            this.Timeout = TimeSpan.FromMilliseconds(timeout);
        }

        public TimeSpan Timeout { get; set; }
    }
}
