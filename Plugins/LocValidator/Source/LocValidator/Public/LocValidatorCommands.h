// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.RuntimeMeshComponentEditor

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CoreStyle.h"

class FLocValidatorCommands : public TCommands<FLocValidatorCommands>
{

public:
	FLocValidatorCommands()
		: TCommands<FLocValidatorCommands>(TEXT("LocValidator"), NSLOCTEXT("Contexts", "FLocValidatorCommands", "LocValidator Plugin"), NAME_None, FCoreStyle::Get().GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;


public:
	TSharedPtr<FUICommandInfo> LocValidateAction;
	TSharedPtr<FUICommandInfo> HelpAction;
};
