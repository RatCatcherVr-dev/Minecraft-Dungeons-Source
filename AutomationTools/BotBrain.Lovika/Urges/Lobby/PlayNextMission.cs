// ------------------------------------------------------------------------------
// <copyright file="PlayNextMission.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.UI;
    using Microsoft.GNS.ObjectModel.Lovika.Extensions;

    /// <summary>
    /// An Urge to use the ingame map to play the mission
    /// </summary>
    public class PlayNextMission : LovikaLobbyUrge
    {
        public override bool IsExclusive => true;

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var player = state.PlayerController(bot.PlayerIndex);
            if (!player.UIInputAllowed)
            {
                return false;
            }

            var missionMap = (await state.GetAll<MissionSelectMap>(TimeSpan.Zero).ConfigureAwait(false))
                .Where(map => map.IsValidObject())
                .FirstOrDefault();

            if (missionMap == null)
            {
                return false;
            }

            if (!missionMap.IsVisible())
            {
                return true;
            }

            if (!missionMap.PlayerControllerInfo.Equals(player.Info))
            {
                return false;
            }

            await state.GetAll<MissionSelectedInspector>(TimeSpan.Zero).ConfigureAwait(false);
            return true;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var missionMap = state.ExistingAll<MissionSelectMap>()
                .Where(map => map.IsValidObject())
                .First();

            if (!missionMap.IsVisible())
            {
                yield return new PlayerInputAction($"Open Mission Map", "Gamepad_DPad_Down");
            }
            else
            {
                var missionSelect = state.ExistingAll<MissionSelectedInspector>()
                    .Where(mission => mission.IsValidObject())
                    .Where(mission => mission.IsVisible())
                    .Where(mission => mission.IsOpen)
                    .FirstOrDefault();

                if (missionSelect == null)
                {
                    yield return new PlayerInputAction($"View Next Mission", "Gamepad_RightTrigger");
                }
                else if (missionSelect.CanStartMission())
                {
                    yield return new PlayerInputAction($"Start Next Mission", "Gamepad_FaceButton_Bottom");
                }
            }
        }
    }
}
