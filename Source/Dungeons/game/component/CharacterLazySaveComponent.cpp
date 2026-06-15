#include "Dungeons.h"
#include "CharacterLazySaveComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "EquipmentComponent.h"


UCharacterLazySaveComponent::UCharacterLazySaveComponent() : Super() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	bReplicates = false;

}

void UCharacterLazySaveComponent::SaveFailed(FString message) {
	OnCharacterSaveFailed.Broadcast(message);

}

void UCharacterLazySaveComponent::OnCharacterAssigned() {
	SetComponentTickEnabled(true);
	if (mInitialized)
		return;

	if (mSaveData) {
		mSaveData->OnSaveFailed.AddDynamic(this, &UCharacterLazySaveComponent::SaveFailed);
		mInitialized = true;
	}

}

void UCharacterLazySaveComponent::FlushCommand() {
	if (mSaveData) {
		if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner()))
		{
			if (auto playerController = playerCharacter->GetPlayerController())
			{
				int32 userId = playerController->GetSaveLocalUserNum();
				if (userId != APlayerControllerBase::INVALID_LOCAL_USER_NUM)
				{
					mSaveData->Save(userId);
				}
			}
		}
	}
}

void UCharacterLazySaveComponent::DirtyItems(int32 inValue) {	
	IncrementDirtyFlag(inValue);
}

void UCharacterLazySaveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	
	//D11.PS - Only save if we have a valid logged in user id
	if (mSaveData) {
		if (auto playerCharacter = Cast<APlayerCharacter>(GetOwner()))
		{
			if (auto playerController = playerCharacter->GetPlayerController())
			{
				int32 userId = playerController->GetSaveLocalUserNum();
				if (userId != APlayerControllerBase::INVALID_LOCAL_USER_NUM)
				{
					mSaveData->Save(userId);
				}
			}
		}
	}
	// D11.SSN
	if (GetWorld()->GetTimerManager().IsTimerActive(saveTimerHandle)) {
		GetWorld()->GetTimerManager().ClearTimer(saveTimerHandle);
	}

	if (mSaveData) {
		mSaveData->OnSaveFailed.RemoveDynamic(this, &UCharacterLazySaveComponent::SaveFailed);
	}
}

// DG: if every character saves to disk, maybe this flushing should happen from a central state?  
void UCharacterLazySaveComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	mTime += DeltaTime;

	// ticking on character
	if (const auto* owner = Cast<APawn>(GetOwner())){
		if (owner->IsLocallyControlled()) {
			const auto dirtyThreshold = 100;
			if (mDirty >= dirtyThreshold) {
				// D11.SSN
				if (auto* playerCharacter = Cast<APlayerCharacter>(GetOwner())) {
					int32 userId = playerCharacter->GetPlayerController()->GetSaveLocalUserNum();
					if (userId != APlayerControllerBase::INVALID_LOCAL_USER_NUM) {
						//d11.gm - makes sure any items that haven't been added to the inventory yet gets added in 
						if (const auto controller = playerCharacter->GetPlayerController()) {
							if (auto stash = controller->GetItemStashComponent()) {
								if (stash->HasDeferredItems()) {
									stash->AddAllDeferredItems();
								}
							}
							mSaveData->mRecordedData.totalGearPower = playerCharacter->GetEquipmentComponent()->GetTotalEquippedDisplayItemPower(); // D11.SSN
						}
						// D11.SSN - don't save more than once every ten seconds.
						if (!saveTimerSet) {
							if (mTime >= 10.0f) {
								Save();
								mTime = 0;
							}
							else {
								GetWorld()->GetTimerManager().SetTimer(saveTimerHandle, this, &UCharacterLazySaveComponent::Save, std::max(10.0f - mTime, 0.1f), false);
								saveTimerSet = true;
							}
						}
					}
				}
				UE_LOG(LogMultiplayer, Log, TEXT("Serializer Flush: Dirty reached: %d which was %f since last flush."), mDirty, mTime);
				mDirty = 0;
			}
		}
	}
}

void UCharacterLazySaveComponent::IncrementDirtyFlag(const int32 inValue) {
	mDirty += FMath::Abs(inValue);
}

// D11.SSN - function used as a timer delegate to prevent multiple saves from occurring in quick succession.
void UCharacterLazySaveComponent::Save() {
	auto* playerCharacter = Cast<APlayerCharacter>(GetOwner());
	int32 userId = playerCharacter->GetPlayerController()->GetSaveLocalUserNum();

	mSaveData->Save(userId);
	if (auto* tracker = playerCharacter->GetStatTracker()) {
		tracker->Save();
	}

	mTime = 0;
	saveTimerSet = false;
}