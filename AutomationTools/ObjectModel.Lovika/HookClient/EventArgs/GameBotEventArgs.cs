// ------------------------------------------------------------------------------
// <copyright file="GameBotEventArgs.cs" company="Microsoft">
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
    using Microsoft.Internal.GamesTest.Rpc.Client;
    
    /// <summary>
    /// Event args for the various bot events. Contains ID of the bot
    /// </summary>
    public class GameBotEventArgs : EventArgs
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="GameBotEventArgs"/> class from a specified RpcArchive.
        /// </summary>
        /// <param name="archive">The archive instance from which to transfer state.</param>
        public GameBotEventArgs(RpcArchive archive)
        {
            this.ReadArchive(archive);
        }

        /// <summary>
        /// Gets the Id of the bot
        /// </summary>
        public string AutomatorId { get; private set; }

        /// <summary>
        /// Gets the player index of the bot.
        /// </summary>
        public int? PlayerIndex { get; private set; }

        protected virtual void ReadArchive(RpcArchive archive)
        {
            this.AutomatorId = archive.GetNamedValue<string>("InputAutomatorId");
            if (archive.HasNamedValue("PlayerIndex"))
            {
                this.PlayerIndex = archive.GetNamedValue<int>("PlayerIndex");
            }
            else
            {
                string prefix = "APlayerAutomator_";
                if (this.AutomatorId != null && this.AutomatorId.StartsWith(prefix, StringComparison.Ordinal))
                {
                    if (int.TryParse(this.AutomatorId.Replace(prefix, string.Empty), out var playerNumber))
                    {
                        this.PlayerIndex = playerNumber - 1;
                    }
                }
            }
        }
    }
}
