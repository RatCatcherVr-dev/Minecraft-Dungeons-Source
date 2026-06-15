//------------------------------------------------------------------------------
// <copyright file="LovikaHookClient.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Net;
    using System.Net.Sockets;
    using System.Numerics;
    using System.Threading;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.UnrealToolkit.HookClient;
    using Microsoft.GNS.UnrealToolkit.HookClient.Extensions;
    using Microsoft.Internal.GamesTest.Rpc.Client;

    /// <summary>
    /// Class containing all game hooks and events.
    /// Derives from the <see cref="UnrealHookClient"/> inheriting all engine hooks.
    /// Partial class containing main logic and game hooks.
    /// </summary>
    public partial class LovikaHookClient : UnrealHookClient, IUE4HookClient
    {
        private readonly ISet<string> arrayTypeProperties = new HashSet<string>();

        /// <summary>
        /// Initializes a new instance of the <see cref="LovikaHookClient"/> class.
        /// </summary>
        /// <param name="connectionString">
        /// IP address where the TDK RPC Server is running.
        /// </param>
        public LovikaHookClient(string connectionString)
            : base(connectionString, new GameExceptionFactory())
        {
        }

        /// <summary>
        /// Executes the remote method with the given input.
        /// </summary>
        /// <typeparam name="T">The return type</typeparam>
        /// <param name="fieldName">The field name.</param>
        /// <param name="method">The method.</param>
        /// <param name="input">The input.</param>
        /// <param name="outputAsType">Will return type when <c>true</c></param>
        /// <param name="token">The token.</param>
        /// <returns>The results.</returns>
        public async Task<T> InvokeHookMethod<T>(string fieldName, string method, RpcArchive input, bool outputAsType = false, CancellationToken token = default(CancellationToken))
        {
            RpcArchive output = await this.InvokeHookMethod(method, input, token).ConfigureAwait(false);
            if (outputAsType)
            {
                return output.ToType<T>();
            }
            else
            {
                return output.GetNamedValue<T>(fieldName);
            }
        }

        /// <summary>
        /// Executes the remote method with the given input.
        /// </summary>
        /// <param name="method">The method.</param>
        /// <param name="input">The input.</param>
        /// <param name="token">The token.</param>
        /// <returns>Task to await.</returns>
        public Task<RpcArchive> InvokeHookMethod(string method, RpcArchive input, CancellationToken token = default(CancellationToken))
        {
            return Task.Run(
                () =>
                {
                    return this.Channel.InvokeRemoteMethod(method, input);
                },
                token);
        }

        /// <summary>
        /// Executes the console command.
        /// </summary>
        /// <param name="command">The command.</param>
        /// <param name="useConsole">if set to <c>true</c> [use console].</param>
        /// <param name="token">The token.</param>
        /// <returns>Task to await.</returns>
        public Task<string> ExecuteConsoleCommand(string command, bool useConsole, CancellationToken token = default(CancellationToken))
        {
            return Task.Run(
                () =>
                {
                    string result = " ";
                    command = command.Replace("\r\n", string.Empty);
                    RpcArchive input = new RpcArchive();
                    try
                    {
                        input.SetNamedValue<string>("Command", command);
                        RpcArchive output = this.Channel.InvokeRemoteMethod("ExecuteCommand", input);
                        result = output.GetNamedValue<string>("CmdOutput");
                    }
                    catch
                    {
                    }

                    return result;
                },
                token);
        }

        /// <summary>
        /// Gets the actor properties.
        /// </summary>
        /// <param name="query">The query.</param>
        /// <param name="properties">The properties.</param>
        /// <param name="token">The token.</param>
        /// <returns>The results.</returns>
        public Task<IEnumerable<QueryResult>> GetActorProperties(ObjectInfo query, IReadOnlyCollection<string> properties, CancellationToken token = default(CancellationToken))
        {
            return Task.Run(
                () =>
                {
                    RpcArchive result = this.Channel.InvokeRemoteMethod("GetActorProperties", this.GetGameStateQuery(query.ClassName, query.ObjectName, properties.ToList()));
                    return result.GetNamedCollection<RpcArchive>("Actors")
                        .SelectMany(archive => this.ConvertArchiveToGameStateQueryResults(archive, query.ClassName))
                        .Select(GetQueryResultFromTuple);
                },
                token);
        }

        /// <summary>
        /// Gets the object properties.
        /// </summary>
        /// <param name="query">The query.</param>
        /// <param name="properties">The properties.</param>
        /// <param name="token">The token.</param>
        /// <returns>The results.</returns>
        public Task<IEnumerable<QueryResult>> GetObjectProperties(ObjectInfo query, IReadOnlyCollection<string> properties, CancellationToken token = default(CancellationToken))
        {
            return Task.Run(
                () =>
                {
                    RpcArchive result = this.Channel.InvokeRemoteMethod("GetObjectProperties", this.GetGameStateQuery(query.ClassName, query.ObjectName, properties.ToList()));
                    return result.GetNamedCollection<RpcArchive>("Objects")
                        .SelectMany(archive => this.ConvertArchiveToGameStateQueryResults(archive, query.ClassName))
                        .Select(GetQueryResultFromTuple);
                },
                token);
        }

        private static QueryResult GetQueryResultFromTuple(Tuple<string, string, KeyValuePair<string, object>> tuple)
        {
            return new QueryResult(new ObjectInfo(tuple.Item1, tuple.Item2), tuple.Item3);
        }

        private RpcArchive GetGameStateQuery(string className, string objectName, IReadOnlyList<string> properties)
        {
            RpcArchive input = new RpcArchive();
            input.SetNamedValue("Properties", properties);
            if (!string.IsNullOrEmpty(className))
            {
                input.SetNamedValue("ClassName", className);
            }

            if (!string.IsNullOrEmpty(objectName))
            {
                input.SetNamedValue("ObjectNames", new List<string>() { objectName });
            }

            return input;
        }

        private IEnumerable<Tuple<string, string, KeyValuePair<string, object>>> ConvertArchiveToGameStateQueryResults(RpcArchive archive, string queryClass)
        {
            const string ArrayItemSeparator = "\n";

            string resultClass = archive.GetNamedValue<string>("class");
            string resultName = archive.GetNamedValue<string>("name");

            foreach (RpcArchiveField field in archive.Fields)
            {
                string propertyName = field.Name;
                if (propertyName == "name" || propertyName == "class")
                {
                    continue;
                }

                ISet<string> propertyKeys = new HashSet<string>() { $"{queryClass}.{propertyName}", $"{resultClass}.{propertyName}" };
                string propertyValue = null;

                // If the field isn't known to be an array type, first try to extract the property from the RpcArchive as a string
                if (this.arrayTypeProperties.Overlaps(propertyKeys) || !archive.TryGetStringValue(propertyName, out propertyValue))
                {
                    // Attempt to extract the property as a string collection
                    string[] items;
                    if (archive.TryGetStringCollection(propertyName, out items))
                    {
                        this.arrayTypeProperties.UnionWith(propertyKeys);
                        propertyValue = string.Join(ArrayItemSeparator, items.OfType<string>());
                    }
                    else
                    {
                        throw new RpcArchiveFieldNotFoundException($"Failed to extract property {propertyName} from RPC archive.");
                    }
                }

                if (propertyValue != null)
                {
                    yield return new Tuple<string, string, KeyValuePair<string, object>>(resultClass, resultName, new KeyValuePair<string, object>(propertyName, propertyValue));
                }
            }
        }
    }
}
