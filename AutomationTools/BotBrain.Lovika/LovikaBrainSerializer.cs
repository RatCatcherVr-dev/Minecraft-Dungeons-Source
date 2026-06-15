// ------------------------------------------------------------------------------
// <copyright file="LovikaBrainSerializer.cs" company="Microsoft">
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
    using Microsoft.GNS.BotBrain.Infrastructure.Composition;
    using Microsoft.GNS.BotBrain.Lovika.Actions;
    using Microsoft.GNS.ObjectModel.Lovika;

    public class LovikaBrainSerializer : BrainSerializer<LovikaGameState, LovikaBot, LovikaBotActionBase, LovikaBrain>
    {
        public override string TitleOutputFolder => "Lovika";
        public override string BrainTemplatesFolderPath => this.GetTitleResourcePath("Templates");

        public override void Initialize()
        {
            base.Initialize();

            this.LoadTemplates<LovikaBrain>(this.BrainTemplatesFolderPath, this.JsonConverters.ToArray());
        }

        protected override void WriteTitleResources() =>
            this.WriteNewResourcesToDisk(assembly: this.GetType().Assembly, baseFolder: "BotBrains", outputFolder: this.TitleOutputFolder);
    }
}
