// ------------------------------------------------------------------------------
// <copyright file="KillMobsInRadius.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to kill mobs around and near the closest villian
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance), nameof(Parameters.AttackRadius))]
    public class KillMobsInRadius : LovikaLevelUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);

            var player = state.PlayerCharacter(bot.PlayerIndex);

            var villains = await state.GetMobVillains(TimeSpan.Zero).ConfigureAwait(false);
            return villains
                .Where(mob => mob.IsTargetable && mob.PlayerVisible)
                .Where(mob => mob.IsInRangeOf(player, maxDistance))
                .Where(mob => mob.EntityType != "Llama")
                .Any();
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var attackRadius = this.GetParameter<float>(bot, Parameters.AttackRadius);

            var player = state.PlayerCharacter(bot.PlayerIndex);

            var closestVillain = state.MobVillains()
                .Where(mob => mob.IsTargetable && mob.PlayerVisible)
                .Where(mob => mob.IsInRangeOf(player, maxDistance))
                .Where(mob => mob.EntityType != "Llama")
                .OrderBy(mob => mob.DistanceTo(player))
                .FirstOrDefault();

            if (closestVillain != null)
            {
                var location = closestVillain.Location().Value;
                yield return new AttackMobsAtLocationAction($"Kill Villains around {location}", location, TeamName.Villains, attackRadius, true);
            }
        }
    }
}
