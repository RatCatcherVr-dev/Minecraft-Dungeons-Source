//------------------------------------------------------------------------------
// <copyright file="UnrealToolsFrameworkModule.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "Engine.h"
#include "ModuleInterface.h"

/** Main external interface for this tools module. */
class IUnrealToolsFrameworkModule : public IModuleInterface, FTickableGameObject
{
public: // IModuleInterface

    /** Called right after the module DLL has been loaded and the module object has been created */
    virtual void StartupModule() = 0;
    
    /** Called before the module is unloaded, right before the module object is destroyed. */
    virtual void ShutdownModule() = 0;

public: // FTickableGameObject

    virtual void Tick(float DeltaTime) = 0;
    virtual TStatId GetStatId() const = 0;

    virtual bool IsTickable() const override { return true; }
    virtual bool IsTickableWhenPaused() const override { return true; }
    virtual bool IsTickableInEditor() const override { return true; }
};
