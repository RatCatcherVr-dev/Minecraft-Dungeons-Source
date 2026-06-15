// ------------------------------------------------------------------------------
// <copyright file="BotActionStatus.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Enumerations
{
    /// <summary>
    /// Enumeration of status codes for bot actions.
    /// </summary>
    public enum BotActionStatus
    {
        Stopped = 0,
        Success,
        InProgress,

        // All further values indicate errors.
        Error,
        ErrorTimedOut
    }
}
