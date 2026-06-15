// ------------------------------------------------------------------------------
// <copyright file="IOnMapLoaded.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Urges
{
    using Microsoft.GNS.ObjectModel.Lovika;
    public interface IOnMapLoaded
    {
        void OnMapLoaded(LovikaGameState state, LovikaBot bot);
    }
}
