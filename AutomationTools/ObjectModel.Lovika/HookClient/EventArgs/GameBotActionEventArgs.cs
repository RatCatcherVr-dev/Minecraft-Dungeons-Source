// ------------------------------------------------------------------------------
// <copyright file="GameBotActionEventArgs.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.EventArgs
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.Internal.GamesTest.Rpc.Client;

    /// <summary>
    /// Event args for the various bot action events. Contains ID of the bot, the action index, and the action status.
    /// </summary>
    public class GameBotActionEventArgs : GameBotEventArgs
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="GameBotActionEventArgs"/> class from a specified RpcArchive.
        /// </summary>
        /// <param name="archive">The archive instance from which to transfer state.</param>
        public GameBotActionEventArgs(RpcArchive archive) : base(archive)
        {
        }

        /// <summary>
        /// Gets or sets the name of the action that was affected
        /// </summary>
        public string ActionName { get; set; }

        /// <summary>
        /// Gets or sets the status of the action
        /// </summary>
        public BotActionStatus ActionStatus { get; set; }

        protected override void ReadArchive(RpcArchive archive)
        {
            base.ReadArchive(archive);
            if (archive.HasNamedValue("ActionName"))
            {
                this.ActionName = archive.GetNamedValue<string>("ActionName");
            }

            if (archive.HasNamedValue("ActionResult"))
            {
                this.ActionStatus = (BotActionStatus)archive.GetNamedValue<int>("ActionResult");
            }
        }
    }
}
