//------------------------------------------------------------------------------
// <copyright file="EquippedItems.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using System.Collections.Generic;
    using System.Runtime.Serialization;

    /// <summary>
    /// A class representing a player's equipped items.
    /// </summary>
    [DataContract]
    public class EquippedItems
    {
        /// <summary>
        /// Gets all the HotBarSlots.
        /// </summary>
        [DataMember]
        public List<string> HotBarSlots;

        /// <summary>
        /// Gets the melee gear equipped.
        /// </summary>
        [DataMember]
        public string MeleeGear { get; private set; }

        /// <summary>
        /// Gets the ranged gear equipped.
        /// </summary>
        [DataMember]
        public string RangedGear { get; private set; }

        /// <summary>
        /// Gets the armor gear equipped.
        /// </summary>
        [DataMember]
        public string ArmorGear { get; private set; }
    }
}
