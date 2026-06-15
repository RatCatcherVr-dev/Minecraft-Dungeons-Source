// ------------------------------------------------------------------------------
// <copyright file="WalkOnJumpTrap.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Concurrent;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Infrastructure;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Cache;
    using Microsoft.GNS.GameStateTracker.Infrastructure.Loggers;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;
    using Microsoft.GNS.UnrealToolkit.HookClient.Exceptions;
    using Parameters = Microsoft.GNS.BotBrain.Lovika.Resources.BotBrains.Parameters;

    /// <summary>
    /// An Urge to walk to the nearest <see cref="JumpTrap"/>
    /// </summary>
    [UsesParameter(nameof(Parameters.MaxDistance), nameof(Parameters.Frequency))]
    public class WalkOnJumpTrap : LovikaLevelUrge, IOnMapLoaded
    {
        private readonly TimeElapsedHelper<LovikaBot> lastUsed = new TimeElapsedHelper<LovikaBot>();

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            this.ClearFailedAttempts(bot);
            state.ClearAll<JumpTrap>();
        }

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!this.CanAttemptGetAll<JumpTrap>(bot))
            {
                return false;
            }

            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var trapFrequency = this.GetParameter<TimeSpan>(bot, Parameters.Frequency);
            if (!this.lastUsed.IsOlderThan(bot, trapFrequency))
            {
                return false;
            }

            var traps = await this.AttemptGetAll<JumpTrap>(state, bot, TimeSpan.FromMinutes(5)).ConfigureAwait(false);
            return traps.Any();
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var maxDistance = this.GetParameter<float>(bot, Parameters.MaxDistance);

            var player = state.PlayerCharacter(bot.PlayerIndex);

            var traps = state.ExistingAll<JumpTrap>()
                .Where(trap => trap.IsInRangeOf(player, maxDistance));

            foreach (var trap in traps)
            {
                var action = new MoveToActorAction($"Move to jump trap {trap}", trap);
                action.OnRun += (object sender, LovikaBot e) => this.lastUsed.Refresh(e);
                yield return action;
            }
        }
    }
}
