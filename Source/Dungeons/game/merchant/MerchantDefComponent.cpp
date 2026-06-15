#include "Dungeons.h"
#include "MerchantDefComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/ShopperComponent.h"
#include <Engine/World.h>
#include "DungeonsGameState.h"

void UMerchantDefComponent::OnPlayerCharacterAdding(APlayerCharacter* player) {
	if (player->IsLocallyControlled() && !HasMerchantFor(player)) {
		CreateMerchantFor(player);
	}
}

void UMerchantDefComponent::OnPlayerCharacterRemoving(APlayerCharacter* player) {
	if(HasMerchantFor(player)){
		DestroyMerchantFor(player);
	}
}

void UMerchantDefComponent::CreateMerchantFor(APlayerCharacter* player) {
	check(player->IsLocallyControlled() && "Player is not locally controlled");
	check(!mPlayerMerchantMap.Find(player) && "Player already in map");
	auto def = MerchantDefinition.GetDefaultObject();
	check(def && "Merchant definition has no class default object");
	auto merchantClass = def->GetMerchantClass();
	check(merchantClass && "No merchant class configured for this merchant");
	if (merchantClass) {
		FTransform transform;
		transform.SetLocation(GetOwner()->GetActorLocation());
		AMerchantBase* merchant = GetWorld()->SpawnActorDeferred<AMerchantBase>(merchantClass, transform, GetOwner(), player, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);		
		merchant->AssignPlayerController(player->GetPlayerController(), def->CreateSaveName(), def->GetDisplayName());
		merchant->EnsureSaveData();
		UGameplayStatics::FinishSpawningActor(merchant, transform);
		if (auto shopper = player->GetPlayerController()->FindComponentByClass<UShopperComponent>()) {
			shopper->EnqueueMerchantWidgetCreationFor(merchant);
		}
		mPlayerMerchantMap.Add(player, merchant);
	}
}

void UMerchantDefComponent::DestroyMerchantFor(APlayerCharacter* player) {
	if (auto merchant = mPlayerMerchantMap.Find(player)) {
		(*merchant)->ClearPlayerController();
		(*merchant)->Destroy();
		mPlayerMerchantMap.Remove(player);
	}
}

void UMerchantDefComponent::BeginPlay() {
	Super::BeginPlay();
	if (auto* gameState = Cast<ADungeonsGameState>(GetWorld()->GetGameState())) {
		gameState->OnPlayerAdding.AddUObject(this, &UMerchantDefComponent::OnPlayerCharacterAdding);
		gameState->OnPlayerRemoving.AddUObject(this, &UMerchantDefComponent::OnPlayerCharacterRemoving);
	}
	for (auto player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
		OnPlayerCharacterAdding(player);
	}
	InstanceTracker<UMerchantDefComponent>::AddInstance(GetWorld(), this);
}

void UMerchantDefComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	InstanceTracker<UMerchantDefComponent>::RemoveInstance(GetWorld(), this);
}


AMerchantBase* UMerchantDefComponent::GetOrCreateMerchantFor(APlayerCharacter* player) {
	check(player->IsLocallyControlled() && "trying to get a merchant for a remote player");
	if (!player->IsLocallyControlled()) { return nullptr; }
	if (!HasMerchantFor(player)) {
		OnPlayerCharacterAdding(player);
	}
	return GetMerchantFor(player);
}

bool UMerchantDefComponent::HasMerchantFor(APlayerCharacter* player) const {
	return mPlayerMerchantMap.Contains(player);
}

AMerchantBase* UMerchantDefComponent::GetMerchantFor(APlayerCharacter* player) const {
	if(auto it = mPlayerMerchantMap.Find(player)){
		return *it;
	}
	return nullptr;
}

bool UMerchantDefComponent::ShouldShowObjectiveMarkerFor(APlayerCharacter* player) const {
	if (auto merchant = GetMerchantFor(player)) {
		return merchant->ShouldShowObjectiveMarker();
	}
	return false;
}

const FText& UMerchantDefComponent::GetMerchantDisplayName() const {
	if(MerchantDefinition){
		if (auto def = MerchantDefinition.GetDefaultObject()) {
			return def->GetDisplayName();
		}
	}
	return FText::GetEmpty();
}
