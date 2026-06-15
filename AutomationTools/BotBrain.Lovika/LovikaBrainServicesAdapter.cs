// ------------------------------------------------------------------------------
// <copyright file="LovikaBrainServicesAdapter.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.Extensions.DependencyInjection;
    using Microsoft.GNS.BotBrain.Infrastructure.Composition;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;

    public class LovikaBrainServicesAdapter : IBrainServicesAdapter
    {
        public static IServiceCollection Apply(IServiceCollection services) =>
            services.AddSingleton<IBrainSerializer, LovikaBrainSerializer>()
                    .AddSingleton<IBrainUpdater, LovikaBrainUpdater>()
                    .AddTransient<IBrainEditor, BrainEditor<LovikaGameState, LovikaBot, LovikaBotActionBase>>()
                    .AddSingleton<IBrainHelper, BrainHelper>();

        /// <inheritdoc/>
        public IServiceCollection RegisterServices(IServiceCollection services) => Apply(services);
    }
}
