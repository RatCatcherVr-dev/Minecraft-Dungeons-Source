// <copyright file="GPUInfo.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using System.Runtime.Serialization;

    /// <summary>
    /// A class representing the GPU related information.
    /// </summary>
    [DataContract]
    public class GPUInfo
    {
        /// <summary>
        /// Gets the GPU.
        /// </summary>
        [DataMember]
        public string GPU { get; private set; }

        /// <summary>
        /// Gets the Graphics Driver Version.
        /// </summary>
        [DataMember]
        public string GraphicsDriverVersion { get; private set; }

        /// <summary>
        /// Gets the GPU Vendor.
        /// </summary>
        [DataMember]
        public string GPUVendor { get; private set; }
    }
}
