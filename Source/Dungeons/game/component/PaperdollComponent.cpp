#include "Dungeons.h"
#include "PaperdollComponent.h"

UPaperdollComponent::UPaperdollComponent() {
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	static const ConstructorHelpers::FObjectFinder<USkeletalMesh> mesh(TEXT("/Game/Actors/Characters/Player/Master/SK_Player_Master"));
	
	if (mesh.Succeeded()) {
		USkeletalMeshComponent::SetSkeletalMesh(mesh.Object);
	}
}

void UPaperdollComponent::AttachToPlayerMesh(USkeletalMeshComponent* playerMesh) {
	if (playerMesh) {
		for (USceneComponent* childComponent : GetChildrenComponents()) {
			childComponent->AttachToComponent(playerMesh, FAttachmentTransformRules::KeepRelativeTransform, childComponent->GetAttachSocketName());
			childComponent->SetCanEverAffectNavigation(false);
			childComponent->SetTickableWhenPaused(playerMesh->PrimaryComponentTick.bTickEvenWhenPaused); //D11.SC follow new parents tick when paused

			UPrimitiveComponent* pPrimitiveComp = Cast<UPrimitiveComponent>(childComponent);

			//Disable Unnecessary overheads for items with no physics. 
			//D11.SC - this is a catch-all to remove all the overheads left on in the gear assets rather than changing every asset to remove them
			if (pPrimitiveComp && !(ComponentsCanUsePhysics && pPrimitiveComp->IsPhysicsCollisionEnabled()) )
			{
				pPrimitiveComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				pPrimitiveComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
				pPrimitiveComp->SetGenerateOverlapEvents(false);
				pPrimitiveComp->SetSimulatePhysics(false);
				pPrimitiveComp->SetEnableGravity(false);
				pPrimitiveComp->bUseAttachParentBound = true;
				pPrimitiveComp->SetShouldUpdatePhysicsVolume(false);
				pPrimitiveComp->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
			}

			OnAttachedToPlayerMesh.Broadcast(childComponent);
		}

		DestroyComponent();
	}
}

TArray<USceneComponent*> UPaperdollComponent::GetChildrenComponents() const {
	TArray<USceneComponent*> components;
	USceneComponent::GetChildrenComponents(false, components);
	return components;
}
