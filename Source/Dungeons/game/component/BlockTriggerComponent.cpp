// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/Conversion.h"
#include "game/component/blocktriggers/SlippyBlockTrigger.h"
#include "game/component/blocktriggers/LavaBlockTrigger.h"
#include "game/component/blocktriggers/WaterBlockTrigger.h"
#include "util/EnumUtil.h"
#include "BlockTriggerComponent.h"

TAutoConsoleVariable<int32> CVarEnableBlockDebugger(
	TEXT("Dungeons.EnableBlockDebugger"),
	0,
	TEXT("Enables on-screen information about the current block being stood on and overlapped with.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

UBlockTriggerComponent::UBlockTriggerComponent(){
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UBlockTriggerComponent::BeginPlay() {
	Super::BeginPlay();	
	Game = actorquery::getFirstActor<AGameBP>(GetWorld());
	const ABaseCharacter* owner = Cast<ABaseCharacter>(GetOwner());
	SetComponentTickEnabled(owner->IsLocallyControlled() || owner->HasAuthority());
}

void UBlockTriggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TryApplyBlockTriggers(DeltaTime);
}


void UBlockTriggerComponent::TryApplyBlockTriggers(float DeltaTime) {
	if (!Game.IsValid()) return;

	const auto& blockTriggers = Game->GetBlockTriggers();
	
	ABaseCharacter* character = Cast<ABaseCharacter>(GetOwner());
	const UMovementComponent* pMoveComp = character->GetMovementComponent();
	const auto blockSource = Game->BlockSourceWithLoadedMaterials();

	if (blockTriggers.Num() && blockSource && pMoveComp && pMoveComp->IsComponentTickEnabled()) {
		const auto capsule = character->GetCapsuleComponent();
		const auto capsuleBottom = capsule->GetComponentLocation() - FVector(0.f, 0.f, capsule->GetScaledCapsuleHalfHeight());
		const FVector offset(0.f, 0.f, 5.0f);

		const BlockPos currentStepPosition = conversion::ueToBlock(capsuleBottom - offset);
		const BlockPos currentOverlapPosition = conversion::ueToBlock(capsuleBottom + offset);

		const FullBlock previousStepBlock = blockSource->getBlockAndData(previousStepPosition);
		const FullBlock currentStepBlock = blockSource->getBlockAndData(currentStepPosition);
		const bool tryStep = previousStepBlock != currentStepBlock;

		const FullBlock previousOverlapBlock = blockSource->getBlockAndData(previousOverlapPosition);
		const FullBlock currentOverlapBlock = blockSource->getBlockAndData(currentOverlapPosition);
		const bool tryOverlap = previousOverlapBlock != currentOverlapBlock;

#if !UE_BUILD_SHIPPING
		if(CVarEnableBlockDebugger.GetValueOnGameThread() > 0) {
			if (const auto pCharacter = Cast<APlayerCharacter>(character)) {
				const FString stepBlockName = currentStepBlock.getBlock().getDescriptionId().c_str();
				const FString overlapBlockName = currentOverlapBlock.getBlock().getDescriptionId().c_str();
				const FString stepBlockMaterial = GetEnumValueToString(BlockGraphicsHelper::getBlock(GetWorld(), currentStepPosition, currentStepBlock.getBlock().getId()).getMaterialType());
				const FString overlapBlockMaterial = GetEnumValueToString(BlockGraphicsHelper::getBlock(GetWorld(), currentOverlapPosition, currentOverlapBlock.getBlock().getId()).getMaterialType());
				GEngine->AddOnScreenDebugMessage((int32)pCharacter->GetUniqueID(), 100.0f, FColor::Green, FString::Printf(TEXT("Step: %s | %i:%i | %s\nOverlap: %s | %i:%i | %s"), *stepBlockName, (uint16_t)currentStepBlock.id, (uint8_t)currentStepBlock.data, *stepBlockMaterial, *overlapBlockName, (uint16_t)currentOverlapBlock.id, (uint8_t)currentOverlapBlock.data, *overlapBlockMaterial));
			}
		}
#endif

		if (tryStep || tryOverlap) {
			EMaterialTypeEnum newStepMaterial = BlockGraphicsHelper::getBlock(GetWorld(), currentStepPosition, currentStepBlock.getBlock().getId()).getMaterialType();
			EMaterialTypeEnum newOverlapMaterial = BlockGraphicsHelper::getBlock(GetWorld(), currentOverlapPosition, currentOverlapBlock.getBlock().getId()).getMaterialType();

			for (TSubclassOf<UBlockTrigger> trigger : blockTriggers) {
				const UBlockTrigger* blockTrigger = trigger.GetDefaultObject();
				if (tryStep) blockTrigger->TryStepUpdate(newStepMaterial, previousStepBlock, currentStepBlock, character, stepTick);
				if (tryOverlap) blockTrigger->TryOverlapUpdate(newOverlapMaterial, previousOverlapBlock, currentOverlapBlock, character, overlapTick);
			}

			if (tryStep) {
				character->SetStepMaterial(newStepMaterial);
				character->SteppedOnNewBlock(currentStepBlock);
			}

			if (tryOverlap) {
				character->SetOverlapMaterial(newOverlapMaterial);
				character->SteppednNewBlock(currentOverlapBlock);
			}
		}

		if (!tryStep) stepTick.Broadcast(DeltaTime, character);
		if (!tryOverlap) overlapTick.Broadcast(DeltaTime, character);

		previousStepPosition = currentStepPosition;
		previousOverlapPosition = currentOverlapPosition;
	}
}

void UBlockTrigger::TryOverlapUpdate(EMaterialTypeEnum& overlapMaterial, const FullBlock& previousOverlapBlock, const FullBlock& currentOverlapBlock, ABaseCharacter* const character, FOnBlockTriggerTick& overlapTick) const {
	bool wasPreviousTrigger = false;
	if (TriggerHasChanged(wasPreviousTrigger, previousOverlapBlock, currentOverlapBlock, character)) {
		if (wasPreviousTrigger) {
			OnExitOverlap(overlapMaterial, character);
			overlapTick.RemoveDynamic(this, &UBlockTrigger::TickOverlap);
		}
		else {
			OnEnterOverlap(overlapMaterial, character);
			overlapTick.AddDynamic(this, &UBlockTrigger::TickOverlap);
		}
	}
}

void UBlockTrigger::TryStepUpdate(EMaterialTypeEnum& stepMaterial, const FullBlock& previousStepBlock, const FullBlock& currentStepBlock, ABaseCharacter* const character, FOnBlockTriggerTick& stepTick) const {
	bool wasPreviousTrigger = false;
	if (TriggerHasChanged(wasPreviousTrigger, previousStepBlock, currentStepBlock, character)) {
		if (wasPreviousTrigger) {
			OnExitStep(stepMaterial, character);
			stepTick.RemoveDynamic(this, &UBlockTrigger::TickStep);
		}
		else {
			OnEnterStep(stepMaterial, character);
			stepTick.AddDynamic(this, &UBlockTrigger::TickStep);
		}
	}
}

bool UBlockTrigger::TriggerHasChanged(bool& wasPreviousTrigger, const FullBlock& previousStepBlock, const FullBlock& currentStepBlock, ABaseCharacter* const character) const {
	wasPreviousTrigger = IsTrigger(previousStepBlock, character);
	const bool isCurrentTrigger = IsTrigger(currentStepBlock, character);
	return wasPreviousTrigger != isCurrentTrigger;
}