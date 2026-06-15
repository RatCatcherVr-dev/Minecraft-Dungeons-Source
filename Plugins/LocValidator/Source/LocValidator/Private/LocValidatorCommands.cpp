// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved. //// todo: update

#include "LocValidatorCommands.h"

#define LOCTEXT_NAMESPACE "LocValidator"

void FLocValidatorCommands::RegisterCommands()
{
	UI_COMMAND(LocValidateAction, "Validate Localization", "Run Loc test and pop up results", EUserInterfaceActionType::Button, FInputGesture());
	UI_COMMAND(HelpAction, "Help", "Documentation of the Loc process steps", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
