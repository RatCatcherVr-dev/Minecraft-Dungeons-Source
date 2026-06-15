// ------------------------------------------------------------------------------
// <copyright file="LobbyAnnouncements.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Info;

    /// <summary>
    /// An Urge to progress through the lobby announcements
    /// </summary>
    public class LobbyAnnouncements : LovikaLobbyUrge, IOnMapLoaded
    {
        private readonly ConcurrentDictionary<string, LastAnnouncement> announcementIndex = new ConcurrentDictionary<string, LastAnnouncement>();

        public override bool IsExclusive => true;

        protected async override Task<bool> Update(LovikaGameState state, LovikaBot bot)
        {
            if (!await base.Update(state, bot).ConfigureAwait(false))
            {
                return false;
            }

            var lobby = state.ExistingAll<LobbyBp>().First();
            if (lobby.AnnouncementsShowed || lobby.AllStartWidgetsAreClosed)
            {
                return false;
            }

            return true;
        }

        protected override IEnumerable<RecommendedAction> RunImpl(LovikaGameState state, LovikaBot bot)
        {
            var announcement = this.announcementIndex.GetOrAdd(bot.Name, new LastAnnouncement());
            yield return new PlayerInputAction($"Progess through lobby announcements ({announcement.Index++})", "Gamepad_FaceButton_Bottom");
        }

        public void OnMapLoaded(LovikaGameState state, LovikaBot bot)
        {
            var announcement = this.announcementIndex.GetOrAdd(bot.Name, new LastAnnouncement());
            announcement.Index = 0;
        }

        private class LastAnnouncement
        {
            public int Index { get; set; }
        }
    }
}
