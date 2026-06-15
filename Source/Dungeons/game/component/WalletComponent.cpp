#include "Dungeons.h"
#include "WalletComponent.h"
#include "ClientEventHub.h"
#include "DungeonsGameInstance.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "EnchantmentComponent.h"
#include "game/actor/character/player/BaseMenuPlayerController.h"
#include "game/ArmorProperties/ArmorPropertiesComponent.h"
#include "game/util/DungeonsGearUtilLibrary.h"
#include "game/item/ItemType.h"
#include "game/merchant/MerchantDefs.h"
#include "game/merchant/MerchantsUtil.h"
#include "online/seasons/LiveOps.h"

UWalletComponent::UWalletComponent()
	: m_initialized(false)
{
	bReplicates = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void UWalletComponent::ClientAdd_Implementation(const FSerializableItemId& type, int32 amount, ECurrencyObtainReason reason) {
	auto* character = Cast<APlayerCharacter>(GetOwner());
	auto* characterSerializer = character->GetCharacterSerializeComponent();

	for (auto i = 0; i < Consumers.Num() && amount != 0; ++i) {
		const auto& consumer = Consumers[i];
		if (consumer.IsBound()) {
			amount -= consumer.Execute(type, amount, reason);
		}
	}

	if (amount == 0) return;

	const int32 balance = characterSerializer->ReadCurrencyFor(type);
	characterSerializer->SetCurrencyFor(type, balance + amount);
	BroadcastOnDisplayCountChanged(type);

	characterSerializer->GetCurrenciesFound().Add(type);

	if (type == game::item::type::Gold.getId()) {
		UMerchantsUtil::UnlockMerchantForAllLocalPlayers(GetWorld(), UPiglinMerchantDef::StaticClass());
	}

	character->GetClientEventHubComponent()->ReceivedCurrency(type, amount);
}

void UWalletComponent::BroadcastOnDisplayCountChanged(const FSerializableItemId& type) {
	if (OnDisplayCountChanged.IsBound()) {
		OnDisplayCountChanged.Broadcast(this);
	}
	if (OnDisplayCountChangedInternal.IsBound()) {
		OnDisplayCountChangedInternal.Broadcast(type);
	}
}

int32 UWalletComponent::GetEmeraldBalance() const {
	return Balance(game::item::type::Emerald.getId());
}

int32 UWalletComponent::GetGoldBalance() const {
	return Balance(game::item::type::Gold.getId());
}

void UWalletComponent::Deduct(const FSerializableItemId& type, int32 amount) {
	const auto* character = Cast<APlayerCharacter>(GetOwner());
	auto* characterSerializer = character->GetCharacterSerializeComponent();

	const int32 balance = characterSerializer->ReadCurrencyFor(type);
	if (balance < amount) {
		const auto& itemType = GetItemRegistry().Get(type);
		UE_LOG(LogDungeons, Warning, TEXT("(%d) not enough currency for transaction: %d < %d"), *itemType.getName(), balance, amount);
		return;
	}

	characterSerializer->SetCurrencyFor(type, balance - amount);

	BroadcastOnDisplayCountChanged(type);
}

const UCharacterSerializeComponent* UWalletComponent::ReadOwningCharacterSerializeComponent() const {
	const auto* character = Cast<APlayerCharacter>(GetOwner());
	return character->GetCharacterSerializeComponent();
}

int32 UWalletComponent::Balance(const FSerializableItemId& type) const {	
	return ReadOwningCharacterSerializeComponent()->ReadCurrencyFor(type);
}

// Called when the game starts
void UWalletComponent::OnLocalPawnPossessed() {
	// extra safeguard, need only load once
	if (m_initialized) {
		return;
	}

	const auto* character = Cast<APlayerCharacter>(GetOwner());
	auto* characterSerialiseComponent = character->GetCharacterSerializeComponent();
	auto strongholdData = characterSerialiseComponent->GetStrongholdData();	
	if (Balance(game::item::type::EyeOfEnder.getId()) < strongholdData.EyesHeldCount()) {
		int difference = strongholdData.EyesHeldCount() - Balance(game::item::type::EyeOfEnder.getId());
		ClientAdd(game::item::type::EyeOfEnder.getId(), difference, ECurrencyObtainReason::Pickup);
	}

	for (const FItemId* itemId : ReadOwningCharacterSerializeComponent()->ReadOwnedCurrencyTypes()) {
		BroadcastOnDisplayCountChanged({ *itemId });
	}

	m_initialized = true;
}


bool UWalletComponent::AddConsumer(const FCurrencyConsumer& consumer) {
	if (!Consumers.ContainsByPredicate([&](const FCurrencyConsumer& c) { return c.GetHandle() == consumer.GetHandle(); })) {
		Consumers.Add(consumer);
		return true;
	}

	return false;
}

bool UWalletComponent::RemoveAllConsumers(const UObject* object) {
	const int count = Consumers.Num();

	Consumers.RemoveAll([&](const FCurrencyConsumer& c) { return c.GetUObject() == object; });

	return count != Consumers.Num();
}
