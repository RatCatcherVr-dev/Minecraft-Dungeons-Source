// ------------------------------------------------------------------------------
// <copyright file="ContextSensitiveWidget.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Extensions
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Player;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;

    /// <summary>
    /// Extension methods to retrieve <see cref="ContextSensitiveWidget" /> <see cref="IGameState" />.
    /// </summary>
    public static partial class IGameStateExtensions
    {
        public static IEnumerable<ContextSensitiveWidget> VisibleContextWidgets(this IGameState self)
        {
            return self.ExistingAll<ContextSensitiveWidget>()
                .Where(w => w.IsValidObject())
                .Where(w => w.IsVisible());
        }

        public static IEnumerable<ContextSensitiveWidget> VisibleContextWidgetsWithOwnership(this IGameState self, PlayerPawn player)
        {
            return self.VisibleContextWidgets()
                .Where(w => w.OwningCharacterInfo.Equals(player.Info));
        }

        public static async Task<IEnumerable<ContextSensitiveWidget>> GetVisibleContextWidgets(this IGameState self, TimeSpan? maxAge = null)
        {
            var allWidgets = await self.GetAll<ContextSensitiveWidget>(maxAge).ConfigureAwait(false);
            var validWidgets = allWidgets
                .Where(w => w.IsValidObject())
                .Where(w => w.IsVisible());

            foreach (var widget in validWidgets)
            {
                await widget.GetInteractableActor<AActor>(maxAge).ConfigureAwait(false);
            }

            return validWidgets;
        }

        public static async Task<IEnumerable<ContextSensitiveWidget>> GetVisibleContextWidgetsWithOwnership(this IGameState self, PlayerPawn player, TimeSpan? maxAge = null)
        {
            var allWidgets = await self.GetVisibleContextWidgets(maxAge).ConfigureAwait(false);
            var validWidgets = allWidgets
                .Where(w => w.OwningCharacterInfo.Equals(player.Info));

            return validWidgets;
        }
    }
}
