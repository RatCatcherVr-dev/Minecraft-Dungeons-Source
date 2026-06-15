// ------------------------------------------------------------------------------
// <copyright file="LoadingScreenActiveException.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Exceptions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Runtime.Serialization;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.UnrealToolkit.HookClient.Exceptions;

    [Serializable]
    public class LoadingScreenActiveException : HookClientException
    {
        public LoadingScreenActiveException()
        {
        }

        public LoadingScreenActiveException(string message) : base(message)
        {
        }

        public LoadingScreenActiveException(string message, Exception innerException) : base(message, innerException)
        {
        }
       

        protected LoadingScreenActiveException(SerializationInfo info, StreamingContext context) : base(info, context)
        {
        }
    }
}
