//------------------------------------------------------------------------------
// <copyright file="GameExceptionFactory.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using Microsoft.GNS.ObjectModel.Lovika.Exceptions;
    using Microsoft.GNS.UnrealToolkit.HookClient.Exceptions;

    /// <summary>
    /// Class that performs mapping of TDK RPC error codes to exceptions.
    /// </summary>
    public class GameExceptionFactory : UnrealExceptionFactory
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="GameExceptionFactory"/> class.
        /// </summary>
        public GameExceptionFactory()
        {
            this.RegisterMapping(1, (m, i, o) => new LoadingScreenActiveException("The loading screen is currently active"));
        }
    }
}
