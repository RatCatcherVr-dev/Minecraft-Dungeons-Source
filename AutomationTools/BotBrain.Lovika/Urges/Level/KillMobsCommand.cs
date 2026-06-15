// ------------------------------------------------------------------------------
// <copyright file="KillMobsCommand.cs" company="Microsoft">
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
    using System.Text.RegularExpressions;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to run the kill mobs command when in range of the player
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance))]
    public class KillMobsCommand : LovikaLevelUrge
    {
        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            return (await state.GetMobVillains(TimeSpan.Zero).ConfigureAwait(false)).Any();
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);
            var player = state.PlayerCharacter(bot.PlayerIndex);

            var mobList = state.MobVillains()
                .Where(mob => mob.IsInRangeOf(player, maxDistance))
                .Where(mob => mob.IsTargetable);

            if (mobList.Any())
            {
                yield return new ConsoleCommandAction($"Kill Mobs in area around {player.Location()} ({DateTime.Now.Ticks})", $"Dungeons.Mob.KillAll");
            }
        }
    }
}
