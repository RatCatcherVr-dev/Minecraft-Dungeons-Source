// ------------------------------------------------------------------------------
// <copyright file="ActivateObjective.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to and activate objectives.
    /// </summary>
    [UsesParameter(nameof(Parameters.GameStateMaxAge))]
    public class ActivateObjective : LovikaInGameUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            TimeSpan gameMaxAge = this.GetParameter<TimeSpan>(bot, Parameters.GameStateMaxAge);
            var gameBp = (await state.GetAll<GameBP>(gameMaxAge).ConfigureAwait(false)).FirstOrDefault();
            if (gameBp == null)
            {
                return false;
            }

            if (gameBp.ObjectiveLocations.Locations.Any() && gameBp.ObjectiveLocations.UseLocations)
            {
                await state.GetVisibleContextWidgets(TimeSpan.Zero).ConfigureAwait(false);
                return true;
            }

            return false;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var player = state.PlayerCharacter(bot.PlayerIndex);
            var visibleWidget = state.VisibleContextWidgetsWithOwnership(player).FirstOrDefault();

            var gameBp = state.ExistingAll<GameBP>().First();
            foreach (var location in gameBp.ObjectiveLocations.Locations)
            {
                if (this.IsWidgetForLocation(visibleWidget, location))
                {
                    var actor = visibleWidget.InteractableActor<AActor>();
                    yield return new PlayerOnFunctionAction($"Activate {actor.Name} @{location}", PlayerOnFunctionType.MeleeAttack);
                }
                else
                {
                    var dist = player.DistanceTo(location);
                    // Cores workaround
                    if (dist <= 250)
                    {
                        yield return new PlayerOnFunctionAction($"Activate close to @{location}", PlayerOnFunctionType.MeleeAttack);
                    }
                    else
                    {
                        yield return new MoveToVector3Action($"Move to objective @{location}", location);
                    }
                }
            }
        }
    }
}
