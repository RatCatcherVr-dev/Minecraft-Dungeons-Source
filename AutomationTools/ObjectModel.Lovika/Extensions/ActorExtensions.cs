// ------------------------------------------------------------------------------
// <copyright file="ActorExtensions.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.UE4;

    public static class ActorExtensions
    {
        public static float DistanceTo(this AActor self, Vector3 target)
        {
            var location = self.Location();
            if (location.HasValue)
            {
                return Vector3.Distance(location.Value, target);
            }

            return -1;
        }

        public static float DistanceTo(this AActor self, AActor target)
        {
            var targetLocation = target.Location();
            if (targetLocation.HasValue)
            {
                return self.DistanceTo(targetLocation.Value);
            }

            return -1;
        }

        public static bool IsInRangeOf(this AActor self, Vector3 target, float radius)
        {
            var location = self.Location();
            if (location.HasValue)
            {
                var distance = self.DistanceTo(target);
                return distance <= radius;
            }

            return false;
        }

        public static bool IsInRangeOf(this AActor self, AActor target, float radius)
        {
            var targetLocation = target.Location();
            if (targetLocation.HasValue)
            {
                return self.IsInRangeOf(targetLocation.Value, radius);
            }

            return false;
        }
    }
}
