//------------------------------------------------------------------------------
// <copyright file="CPUInfo.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using System.Runtime.Serialization;

    /// <summary>
    /// A class representing the CPU information.
    /// </summary>
    [DataContract]
    public class CPUInfo
    {
        /// <summary>
        /// Gets the CPU.
        /// </summary>
        [DataMember]
        public string CPU { get; private set; }

        /// <summary>
        /// Gets the number of cores.
        /// </summary>
        [DataMember]
        public int CPUCount { get; private set; }
    }
}
