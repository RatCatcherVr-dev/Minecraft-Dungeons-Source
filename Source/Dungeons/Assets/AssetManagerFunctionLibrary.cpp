// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetManagerFunctionLibrary.h"
#include "DungeonsAssetManager.h"
#include "DungeonsInventoryAssetManager.h"

UDungeonsAssetManager* UAssetManagerFunctionLibrary::GetDefaultAssetManager()
{
	return Cast<UDungeonsAssetManager>(UDungeonsAssetManager::StaticClass()->GetDefaultObject());
}

UDungeonsInventoryAssetManager* UAssetManagerFunctionLibrary::GetDefaultInventoryAssetManager()
{
	return Cast<UDungeonsInventoryAssetManager>(UDungeonsInventoryAssetManager::StaticClass()->GetDefaultObject());
}

class UStaticMeshComponent* UAssetManagerFunctionLibrary::CreateHiddenShadowMeshCopy(UStaticMeshComponent* sourceComponent)
{
	if (sourceComponent)
	{
		FName NewName = *(sourceComponent->GetName() + TEXT("_HiddenShadow"));
		
		UStaticMeshComponent* NewComponent = DuplicateObject<UStaticMeshComponent>(sourceComponent, sourceComponent->GetOwner(), NewName);		
		NewComponent->SetMobility(EComponentMobility::Stationary);
		NewComponent->ResetRelativeTransform();
		NewComponent->SetCanEverAffectNavigation(false);
		NewComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		NewComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewComponent->bCastHiddenShadow = 1;
		NewComponent->bHiddenInGame = 1;
		NewComponent->AttachToComponent(sourceComponent, FAttachmentTransformRules::KeepRelativeTransform);		
		sourceComponent->GetOwner()->AddInstanceComponent(NewComponent);
		NewComponent->RegisterComponent();

		return NewComponent;
	}
	return nullptr;
}
