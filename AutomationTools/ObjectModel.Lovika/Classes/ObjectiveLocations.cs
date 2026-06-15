// ------------------------------------------------------------------------------
// <copyright file="ObjectiveLocations.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure.TreeObject;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.GameToolkit;

    public struct ObjectiveLocations : IEquatable<ObjectiveLocations>
    {
        public ObjectiveLocations(IEnumerable<Vector3> locations, bool useLocations)
        {
            this.Locations = locations;
            this.UseLocations = useLocations;
        }

        public ObjectiveLocations(IEnumerable<Vector3> locations) : this(locations, locations.Any())
        {
        }

        public IEnumerable<Vector3> Locations { get; private set; }

        public bool UseLocations { get; private set; }

        public static bool operator ==(ObjectiveLocations left, ObjectiveLocations right) => left.Equals(right);

        public static bool operator !=(ObjectiveLocations left, ObjectiveLocations right) => !left.Equals(right);

        /// <summary>
        /// Tries to parse the input string as a <see cref="ObjectiveLocations"/> object.
        /// </summary>
        /// <param name="input">The string to parse.</param>
        /// <returns>A objective locations object</returns>
        public static ObjectiveLocations? TryParse(string input)
        {
            var rootTree = Parser.TryParse(input) as Node;
            if (rootTree == null)
            {
                return null;
            }

            var useLocations = false;
            var locations = new List<Vector3>();

            foreach (var node in rootTree)
            {
                if (node.Key == "Locations" && node.Value is Node)
                {
                    foreach (var vectorNode in node.Value as Node)
                    {
                        var vector = UE4Helpers.ParseTreeObjectLocation(vectorNode.Value as Node);
                        if (vector != default)
                        {
                            locations.Add(vector);
                        }
                    }
                }
                else if (node.Key == "bUseLocations" && node.Value is Leaf)
                {
                    useLocations = NumericHelpers.ParseBool(node.Value.Value) ?? false;
                }
            }

            return new ObjectiveLocations(locations, useLocations);
        }

        public bool Equals(ObjectiveLocations other)
        {
            return other.UseLocations == this.UseLocations
                && other.Locations.SequenceEqual(this.Locations);
        }
    }
}
