// ------------------------------------------------------------------------------
// <copyright file="MobCharacter.cs" company="Microsoft">
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
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    /// <summary>
    /// Extension methods to retrieve <see cref="MobCharacter" /> <see cref="IGameState" />.
    /// </summary>
    public static partial class IGameStateExtensions
    {
        public static IEnumerable<MobCharacter> MobCharactersByTeamName(this IGameState self, TeamName teamName)
        {
            return self.ExistingAll<MobCharacter>()
                .Where(m => m.TeamName == teamName);
        }

        public static async Task<IEnumerable<MobCharacter>> GetMobCharactersByTeamName(this IGameState self, TeamName teamName, TimeSpan? maxAge = null)
        {
            var mobCharacters = await self.GetAll<MobCharacter>(maxAge).ConfigureAwait(false);

            return mobCharacters
                .Where(m => m.TeamName == teamName);
        }

        public static IEnumerable<MobCharacter> MobKeys(this IGameState self)
        {
            return self.MobCharactersByTeamName(TeamName.Keys);
        }

        public static async Task<IEnumerable<MobCharacter>> GetMobKeys(this IGameState self, TimeSpan? maxAge = null)
        {
            var silverKeyCharacter = self.GetAll<MobCharacter>("BP_SilverBabyKeyCharacter_C", maxAge);
            var goldKeyCharacter = self.GetAll<MobCharacter>("BP_GoldBabyKeyCharacter_C", maxAge);

            var mobCharacters = await Task.WhenAll(silverKeyCharacter, goldKeyCharacter).ConfigureAwait(false);
            return mobCharacters
                .SelectMany(list => list)
                .Where(mob => mob.TeamName == TeamName.Keys);
        }

        public static IEnumerable<MobCharacter> MobVillains(this IGameState self)
        {
            return self.MobCharactersByTeamName(TeamName.Villains);
        }

        public static Task<IEnumerable<MobCharacter>> GetMobVillains(this IGameState self, TimeSpan? maxAge = null)
        {
            return self.GetMobCharactersByTeamName(TeamName.Villains, maxAge);
        }

        public static IEnumerable<MobCharacter> DestructableLootUrns(this IGameState self)
        {
            // FIXME
            return self.ExistingAll<MobCharacter>("BP_DestructableLootUrn_C");
        }

        public static Task<IEnumerable<MobCharacter>> GetDestructableLootUrns(this IGameState self, TimeSpan? maxAge = null)
        {
            // FIXME
            return self.GetAll<MobCharacter>("BP_DestructableLootUrn_C", maxAge);
        }
    }
}
