// ------------------------------------------------------------------------------
// <copyright file="LovikaHookClient.Hooks.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

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
    using Microsoft.GNS.GameStateTracker.UE4;
    using Microsoft.GNS.ObjectModel.Lovika.Classes.Actor;
    using Microsoft.GNS.ObjectModel.Lovika.Enumerations;
    using Microsoft.GNS.UnrealToolkit.HookClient;
    using Microsoft.Internal.GamesTest.Rpc.Client;

    /// <summary>
    /// Class containing all game hooks and events.
    /// Derives from the <see cref="UnrealHookClient"/> inheriting all engine events.
    /// Partial class containing game hooks.
    /// </summary>
    public partial class LovikaHookClient
    {
        public Task AddAutomation(int playerIndex)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            return this.InvokeHookMethod("AddAutomation", input);
        }

        public Task RemoveAutomation(int playerIndex)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            return this.InvokeHookMethod("RemoveAutomation", input);
        }

        public Task ClearActions(int playerIndex)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            return this.InvokeHookMethod("ClearActions", input);
        }

        public Task PlayerInputKey(int playerIndex, string name, string key, bool viewport)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Name", name);
            input.SetNamedValue("Key", key);
            input.SetNamedValue("Viewport", viewport);
            return this.InvokeHookMethod("KeyPressAction", input);
        }

        public Task PlayerOnFunctionAction(int playerIndex, string name, PlayerOnFunctionType action, int index)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Name", name);
            input.SetNamedValue("Action", (int)action);
            input.SetNamedValue("Index", index);
            return this.InvokeHookMethod("OnFunctionAction", input);
        }

        public Task MovePlayerToLocaion(int playerIndex, string name, Vector3 location, bool pathFinding, float acceptanceRadius)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Name", name);
            input.SetNamedValue("Location", location);
            input.SetNamedValue("PathFinding", pathFinding);
            input.SetNamedValue("AcceptanceRadius", acceptanceRadius);
            return this.InvokeHookMethod("MoveToLocationAction", input);
        }

        public Task MovePlayerToActor(int playerIndex, string name, AActor actor, bool pathFinding, float acceptanceRadius)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Name", name);
            input.SetNamedValue("ClassName", actor.Info.ClassName);
            input.SetNamedValue("ObjectName", actor.Info.ObjectName);
            input.SetNamedValue("PathFinding", pathFinding);
            input.SetNamedValue("AcceptanceRadius", acceptanceRadius);
            return this.InvokeHookMethod("MoveToActorAction", input);
        }

        public Task PlayerAttackMobAttack(int playerIndex, string name, BaseCharacter actor, bool allowRanged)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Name", name);
            input.SetNamedValue("ClassName", actor.Info.ClassName);
            input.SetNamedValue("ObjectName", actor.Info.ObjectName);
            input.SetNamedValue("AllowRanged", allowRanged);
            return this.InvokeHookMethod("PlayerAttackMob", input);
        }

        public Task PlayerAttackRadiusAttack(int playerIndex, string name, Vector3 location, TeamName attackableTeam, float attackRadius, bool allowRanged)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Name", name);
            input.SetNamedValue("Location", location);
            input.SetNamedValue("Team", (int)attackableTeam);
            input.SetNamedValue("Radius", attackRadius);
            input.SetNamedValue("AllowRanged", allowRanged);
            return this.InvokeHookMethod("PlayerAttackRadius", input);
        }

        public Task SetGamepadActive(int playerIndex, bool active)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            input.SetNamedValue("Active", active);
            return this.InvokeHookMethod("SetGamepadActive", input);
        }

        public Task<Vector3> GetNextTileDoor(int playerIndex)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            return this.InvokeHookMethod<Vector3>("Location", "GetNextTileDoor", input);
        }

        public Task<Vector3> GetNextDoor(int playerIndex)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            return this.InvokeHookMethod<Vector3>("Location", "GetNextDoor", input);
        }

        public Task<Vector3> GetNextDoorIndicatorDoor(int playerIndex)
        {
            var input = new RpcArchive();
            input.SetNamedValue("PlayerIndex", playerIndex);
            return this.InvokeHookMethod<Vector3>("Location", "GetNextDoorIndicatorDoor", input);
        }
    }
}
