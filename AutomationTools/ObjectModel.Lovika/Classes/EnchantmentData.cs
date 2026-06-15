//------------------------------------------------------------------------------
// <copyright file="EnchantmentData.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using System.Runtime.Serialization;

    /// <summary>
    /// A class containig the data associated with an Enchantment.
    /// </summary>
    [DataContract]
    public class EnchantmentData
    {
        /// <summary>
        /// Gets the type of enchantment.
        /// </summary>
        [DataMember]
        public string Name { get; private set; }

        /// <summary>
        /// Gets the current level of the enchantment.
        /// </summary>
        [DataMember]
        public int Level { get; private set; }
    }
}
