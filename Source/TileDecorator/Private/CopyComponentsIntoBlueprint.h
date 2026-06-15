#pragma once

#include "CoreMinimal.h"

class AActor;
class USelection;

class FCopyComponentsIntoBlueprint {
public:
	enum class EOpenDialogSave { OpenEditor, CloseEditorAndSave };
	static void OpenDialog(EOpenDialogSave, USelection* selection = nullptr);
	static void OpenDialog(EOpenDialogSave, const TArray<AActor*>&);

	static UObject* CopyBlueprintsTo(const FString& filename, USelection* selection = nullptr);
	static UObject* CopyBlueprintsTo(const FString& filename, const TArray<AActor*>&);
};
