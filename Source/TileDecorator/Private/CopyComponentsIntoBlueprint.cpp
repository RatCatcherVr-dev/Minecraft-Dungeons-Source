// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TileDecorator.h"
#include "CopyComponentsIntoBlueprint.h"
#include "GameFramework/Actor.h"
#include "Misc/PackageName.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/TabManager.h"
#include "Editor/EditorEngine.h"
#include "Editor.h"
#include "SCreateAssetFromObject.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "TileDecoratorUtils.h"
#include <AssetToolsModule.h>
#include <Engine/SCS_Node.h>
#include <AssetRegistryModule.h>
#include <AssetEditorManager.h>

#define LOCTEXT_NAMESPACE "CopyComponentsIntoBlueprint"

namespace {
	UObject* InternalHarvestComponents(const FString& Path, const TArray<AActor*>& SelectedActors) {

		UBlueprint* Blueprint;
		USimpleConstructionScript* SCS;

		if (SelectedActors.Num() > 0)
		{
			// Create a blueprint
			FString PackageName = Path;
			FString AssetName = FPackageName::GetLongPackageAssetName(Path);
			FString BasePath = PackageName + TEXT("/") + AssetName;

			// If no AssetName was found, generate a unique asset name.
			if(AssetName.Len() == 0)
			{
				BasePath = PackageName + TEXT("/") + LOCTEXT("BlueprintName_Default", "NewBlueprint").ToString();
				FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
				AssetToolsModule.Get().CreateUniqueAssetName(BasePath, TEXT(""), PackageName, AssetName);
			}

			UPackage* Package = CreatePackage(nullptr, *PackageName);
			Blueprint = FKismetEditorUtilities::CreateBlueprint(AActor::StaticClass(), Package, *AssetName, BPTYPE_Normal, UBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), FName("HarvestFromActors"));

			check(Blueprint->SimpleConstructionScript != NULL);
			SCS = Blueprint->SimpleConstructionScript;

			// Create a common root if necessary
			USCS_Node* RootNodeOverride = nullptr;

			// Add a new scene component to serve as the shared root node
			RootNodeOverride = SCS->CreateNode(USceneComponent::StaticClass(), TEXT("SharedRoot"));
			Cast<USceneComponent>(RootNodeOverride->ComponentTemplate)->SetMobility(EComponentMobility::Static);
			SCS->AddNode(RootNodeOverride);

			// Harvest the components from each actor and clone them into the SCS
			for (const AActor* Actor : SelectedActors)
			{
				TArray<UActorComponent*> Components;
				Actor->GetComponents(Components);

				// Exclude any components created by other components
				for (int32 Index = Components.Num() - 1; Index >= 0; --Index)
				{
					UActorComponent* ComponentToConsider = Components[Index];
					if (ComponentToConsider->IsEditorOnly())
					{
						Components.RemoveAt(Index, 1, false);
					}
				}

				FKismetEditorUtilities::AddComponentsToBlueprint(Blueprint, Components, true, RootNodeOverride, true);
			}

			// Compute the average origin for all the actors, so it can be backed out when saving them in the blueprint
			FTransform NewActorTransform = FTransform::Identity;
			{
				// Find average location of all selected actors
				FVector AverageLocation = FVector::ZeroVector;
				for (const AActor* Actor : SelectedActors)
				{
					if (USceneComponent* RootComponent = Actor->GetRootComponent())
					{
						AverageLocation += Actor->GetActorLocation();
					}
				}
				AverageLocation /= (float)SelectedActors.Num();

				// Spawn the new BP at that location
				NewActorTransform.SetTranslation(AverageLocation);
			}

			// Reposition all of the children of the root node to recenter them around the new pivot
			for (USCS_Node* TopLevelNode : SCS->GetRootNodes())
			{
				if (USceneComponent* TestRoot = Cast<USceneComponent>(TopLevelNode->ComponentTemplate))
				{
					for (USCS_Node* ChildNode : TopLevelNode->GetChildNodes())
					{
						if (USceneComponent* ChildComponent = Cast<USceneComponent>(ChildNode->ComponentTemplate))
						{
							const FTransform OldChildToWorld(ChildComponent->RelativeRotation, ChildComponent->RelativeLocation);
							const FTransform NewRelativeTransform = OldChildToWorld.GetRelativeTransform(NewActorTransform);
							ChildComponent->RelativeLocation = NewRelativeTransform.GetLocation();
							ChildComponent->RelativeRotation = NewRelativeTransform.GetRotation().Rotator();
						}
					}
				}
			}

			// Regenerate skeleton class as components have been added since initial generation
			FKismetEditorUtilities::GenerateBlueprintSkeleton(Blueprint, true); 
			
			// Notify the asset registry
			FAssetRegistryModule::AssetCreated(Blueprint);

			// Mark the package dirty
			Package->MarkPackageDirty();

			// Open the editor for the new blueprint
			FAssetEditorManager::Get().OpenEditorForAsset(Blueprint);

			return Blueprint;
		}

		return nullptr;
	}
}

void FCopyComponentsIntoBlueprint::OpenDialog(EOpenDialogSave save, USelection* selection /* = nullptr */) {
	OpenDialog(save, util::editor::getSelectedActors(selection));
}

void FCopyComponentsIntoBlueprint::OpenDialog(EOpenDialogSave save, const TArray<AActor*>& actors) {
	if (actors.Num() == 0) {
		return;
	}
	TSharedPtr<SWindow> PickBlueprintPathWidget;
	SAssignNew(PickBlueprintPathWidget, SWindow)
		.Title(LOCTEXT("SelectPath", "Select Path"))
		.ToolTipText(LOCTEXT("SelectPathTooltip", "Select the path where the Blueprint will be created at"))
		.ClientSize(FVector2D(400, 400));

	TSharedPtr<SCreateAssetFromObject> FCopyComponentsIntoBlueprintDialog;
	PickBlueprintPathWidget->SetContent
	(
		SAssignNew(FCopyComponentsIntoBlueprintDialog, SCreateAssetFromObject, PickBlueprintPathWidget)
		.AssetFilenameSuffix(TEXT("Blueprint"))
		.HeadingText(LOCTEXT("CreateBlueprintFromActor_Heading", "Blueprint Name"))
		.CreateButtonText(LOCTEXT("CreateBlueprintFromActor_ButtonLabel", "Create Blueprint"))
		.OnCreateAssetAction(FOnPathChosen::CreateLambda([save, actors](auto& filename) {
			auto object = FCopyComponentsIntoBlueprint::CopyBlueprintsTo(filename, actors);
			if (object && save == EOpenDialogSave::CloseEditorAndSave) {
				util::editor::closeAllEditorsForAsset(object);
				util::save::saveObject(object);
			}
		}))
	);

	TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
	if (RootWindow.IsValid()) {
		FSlateApplication::Get().AddWindowAsNativeChild(PickBlueprintPathWidget.ToSharedRef(), RootWindow.ToSharedRef());
	} else {
		FSlateApplication::Get().AddWindow(PickBlueprintPathWidget.ToSharedRef());
	}
}

UObject* FCopyComponentsIntoBlueprint::CopyBlueprintsTo(const FString& filename, USelection* selection) {
	return FCopyComponentsIntoBlueprint::CopyBlueprintsTo(filename, util::editor::getSelectedActors(selection));
}

UObject* FCopyComponentsIntoBlueprint::CopyBlueprintsTo(const FString& filename, const TArray<AActor*>& actors) {
	bool subclassSingleActor = actors.Num() == 1;
	auto blueprint = subclassSingleActor ? FKismetEditorUtilities::CreateBlueprintFromActor(filename, actors[0], false) : InternalHarvestComponents(filename, actors);
	if (blueprint) {
		TArray<UObject*> objects = TArray<UObject*>{ blueprint };
		GEditor->SyncBrowserToObjects(objects, false);
		return blueprint;
	} else {
		FNotificationInfo info(LOCTEXT("CreateBlueprintFromActorFailed", "Unable to create a blueprint from actor."));
		info.ExpireDuration = 3.0f;
		info.bUseLargeFont = false;
		TSharedPtr<SNotificationItem> notification = FSlateNotificationManager::Get().AddNotification(info);
		if (notification.IsValid()) {
			notification->SetCompletionState(SNotificationItem::CS_Fail);
		}
		return nullptr;
	}
}

#undef LOCTEXT_NAMESPACE
