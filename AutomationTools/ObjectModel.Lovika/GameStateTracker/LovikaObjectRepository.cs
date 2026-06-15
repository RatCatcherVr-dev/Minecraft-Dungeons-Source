//------------------------------------------------------------------------------
// <copyright file="LovikaObjectRepository.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using Microsoft.GNS.GameStateTracker.UE4;

    /// <summary>
    /// The Lovika object repository using UE4ObjectRepositoryWithHooks.
    /// </summary>
    public class LovikaObjectRepository : UE4ObjectRepositoryWithHooks
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="LovikaObjectRepository"/> class.
        /// </summary>
        /// <param name="client">The game client.</param>
        public LovikaObjectRepository(LovikaHookClient client)
            : base(client)
        {
            this.IncludeBlueprintClasses = true;
        }
    }
}
