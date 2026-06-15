//------------------------------------------------------------------------------
// <copyright file="LovikaBotActionBase.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.BotBrain.Lovika.Actions
{
    using System;
    using System.Runtime.Serialization;
    using System.Threading.Tasks;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;

    /// <summary>
    /// An abstract base class for actions which can be run by the Lovika bot.
    /// </summary>
    [DataContract]
    public abstract class LovikaBotActionBase : IEquatable<LovikaBotActionBase>
    {
        private const string NoActionName = "None";

        /// <summary>
        /// Gets the action's name.
        /// </summary>
        [DataMember]
        public virtual string Name { get; protected set; }

        /// <summary>
        /// Gets the bot action's status.
        /// </summary>
        [DataMember]
        public virtual BotActionStatus Status { get; set; } = BotActionStatus.Stopped;

        public virtual bool IsWorkaround { get; set; } = false;

        /// <summary>
        /// Converts a <see cref="LovikaBotActionBase" /> into a <see cref="RecommendedAction" />.
        /// </summary>
        /// <param name="action">The action to be recommended.</param>
        public static implicit operator RecommendedAction(LovikaBotActionBase action)
        {
            return new RecommendedAction(action);
        }

        /// <summary>
        /// Runs the action with the specified bot.
        /// </summary>
        /// <param name="bot">The bot.</param>
        /// <returns>A task which completes once the action has run.</returns>
        public abstract Task Run(LovikaBot bot);

        /// <summary>
        /// Determines if the bot actions are equal.
        /// </summary>
        /// <param name="other">The bot action to compare to.</param>
        /// <returns>True if the bot actions are equal.</returns>
        public virtual bool Equals(LovikaBotActionBase other) => string.Equals(this.ToString(), other?.ToString(), StringComparison.InvariantCulture);

        /// <inheritdoc />
        public override bool Equals(object obj) => this.Equals(obj as LovikaBotActionBase);

        /// <summary>
        /// Returns the action's name.
        /// </summary>
        /// <returns>A string representation of the action.</returns>
        public override string ToString() => this.Name ?? NoActionName;

        /// <inheritdoc/>
        public override int GetHashCode() => base.GetHashCode();
    }
}
