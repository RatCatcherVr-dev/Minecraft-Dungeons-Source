//------------------------------------------------------------------------------
// <copyright file="LovikaGameState.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Cache;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;
    using Microsoft.GNS.GameStateTracker.UE4;

    /// <summary>
    /// A class representing a Lovika game state.
    /// </summary>
    public class LovikaGameState : UE4GameState
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="LovikaGameState"/> class.
        /// </summary>
        /// <param name="lovikaObjectRepository">The Lovika object repository.</param>
        /// <param name="classManager">The Lovika class manager.</param>
        /// <param name="logger">The logger to use.</param>
        /// <param name="cache">The cache to use to store game objects.</param>
        public LovikaGameState(LovikaObjectRepository lovikaObjectRepository, IClassManager classManager = null, ILogger logger = null, IGameObjectCache cache = null)
            : base(lovikaObjectRepository, classManager ?? new LovikaClassManager(), logger, cache)
        {
        }
    }
}
