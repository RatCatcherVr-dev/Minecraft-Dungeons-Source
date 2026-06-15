// ------------------------------------------------------------------------------
// <copyright file="ContextSensitiveWidget.cs" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
// ------------------------------------------------------------------------------

namespace Microsoft.GNS.ObjectModel.Lovika.Classes.UI
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Numerics;
    using System.Text;
    using System.Threading.Tasks;
    using Microsoft.GNS.GameStateTracker.Generic;
    using Microsoft.GNS.GameStateTracker.Infrastructure;
    using Microsoft.GNS.GameStateTracker.UE4;

    [GameClass("ContextSensitiveWidget_C")]
    [GameProperty("OwningCharacter", "InteractableActor")]
    public class ContextSensitiveWidget : UserWidget
    {
        public ContextSensitiveWidget(IGameObject obj) : base(obj)
        {
        }

        public ObjectInfo InteractableActorInfo => this.LinkedObjectInfo("InteractableActor");

        public ObjectInfo OwningCharacterInfo => this.LinkedObjectInfo("OwningCharacter");

        public T InteractableActor<T>() where T : AActor
        {
            return this.GameState.Existing<T>(this.InteractableActorInfo);
        }

        public Task<T> GetInteractableActor<T>(TimeSpan? maxAge = null) where T : AActor
        {
            return this.GameState.Get<T>(this.InteractableActorInfo, maxAge);
        }

        public override bool IsValidObject()
        {
            return base.IsValidObject()
                && this.InteractableActorInfo != default;
        }

        public Vector3? Location()
        {
            return this.InteractableActor<AActor>()?.Location();
        }
    }
}
