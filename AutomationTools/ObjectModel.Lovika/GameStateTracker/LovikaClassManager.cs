// -----------------------------------------------------------------------
// <copyright file="LovikaClassManager.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// -----------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;

    /// <summary>
    /// A class used to associate string class names with custom types and a set of properties.
    /// It automatically adds properties specified by a base class to each of its subclasses.
    /// </summary>
    public class LovikaClassManager : UE4ClassManager
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="LovikaClassManager"/> class.
        /// </summary>
        public LovikaClassManager()
        {
            this.ImportSubclasses<UObject>(typeof(LovikaClassManager).Assembly);
        }
    }
}
