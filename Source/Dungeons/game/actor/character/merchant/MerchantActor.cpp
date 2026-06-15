#include "Dungeons.h"
#include "game/component/InteractableComponent.h"
#include "game/merchant/MerchantDefComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <Components/SkeletalMeshComponent.h>
#include <Animation/AnimInstance.h>
#include "MerchantActor.h"
#include "game/component/ShopperComponent.h"

AMerchantActor::AMerchantActor() {
	bAlwaysRelevant = true;
	NetUpdateFrequency = 10.0f;
	PrimaryActorTick.bCanEverTick = false;
	
	mRootSceneComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	mRootSceneComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(mRootSceneComponent);

	mInteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interactable"));
	mInteractableComponent->SetType(EClickyEnum::CE_Merchant);
	mInteractableComponent->SetBalloontipBehavior(EBalloonTipBehavior::AlwaysVisible);
	mInteractableComponent->AttachTo(mRootSceneComponent);

	mMerchantDefComponent = CreateDefaultSubobject<UMerchantDefComponent>(TEXT("MerchantDef"));
	mMerchantDefComponent->bEditableWhenInherited = true;
}

void AMerchantActor::BeginPlay() {
	Super::BeginPlay();
	mMeshComponent = FindComponentByClass<USkeletalMeshComponent>();
	mInteractableComponent->OnInteracted.AddDynamic(this, &AMerchantActor::HandleInteraction);
	SetCapture3DView(false);
}

bool AMerchantActor::IsMerchantForMatching(class ACharacter* interactor, MerchantPredicate pred) const {
	if (auto player = Cast<APlayerCharacter>(interactor)) {
		return pred(*GetMerchantDefComponent()->GetOrCreateMerchantFor(player));
	}
	return false;
}

void AMerchantActor::SimulateInteraction(class ACharacter* interactor) {
	HandleInteraction(interactor);
}

TSoftClassPtr<class UMerchantWidgetBase> AMerchantActor::GetMerchantWidgetClass() const {
	return mMerchantWidgetClass;
}

void AMerchantActor::HandleInteraction(class ACharacter* interactor) {
	if (auto player = Cast<APlayerCharacter>(interactor)) {
		if (auto merchant = GetMerchantDefComponent()->GetOrCreateMerchantFor(player)) {
			merchant->OnPlayerInteraction(player);		
		}
	}
}

void AMerchantActor::PlayAnimation(UAnimSequenceBase* animation) const {
	if (animation && mMeshComponent) {
		if (mMeshComponent->GetAnimationMode() == EAnimationMode::AnimationBlueprint) {
			auto* animInstance = mMeshComponent->GetAnimInstance();
			animInstance->PlaySlotAnimationAsDynamicMontage(animation, mAnimationSlot);
		}
		else {
			mMeshComponent->PlayAnimation(animation, false);
		}
	}
}

UMerchantDefComponent* AMerchantActor::GetMerchantDefComponent() const {
	return mMerchantDefComponent;
}

