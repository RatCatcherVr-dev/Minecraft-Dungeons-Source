#include "Dungeons.h"
#include "KillTrackerComponent.h"
#include "ClientEventHub.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "world/entity/EntityClassTree.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "world/entity/MobTags.h"

UKillTrackerComponent::UKillTrackerComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UKillTrackerComponent::HandleMobKill(AActor* item, AActor* player, AActor* mob) {
	if (player->IsA<APlayerCharacter>() && mob->IsA<AMobCharacter>()) {
		AMobCharacter* mobCharacter = Cast<AMobCharacter>(mob);		
		
		if (mobCharacter->HasTag(MobTags::HashTag_Miniboss) || mobCharacter->HasTag(MobTags::HashTag_Ancient)) {
			TArray<APlayerCharacter*> playerCharacters = Cast<UDungeonsGameInstance>(GetOwner()->GetGameInstance())->GetAllPlayerCharacters();
			for (APlayerCharacter* playerCharacter : playerCharacters) {
				if (UKillTrackerComponent* tracker = Cast<UKillTrackerComponent>(playerCharacter->GetComponentByClass(UKillTrackerComponent::StaticClass()))) {
					tracker->Client_RegisterKill(mobCharacter->EntityType, mobCharacter->IsEnchanted());
				}
			}
		}
		else {
			Client_RegisterKill(mobCharacter->EntityType, mobCharacter->IsEnchanted());
		}
	}
}

int32 UKillTrackerComponent::GetNumKillsInCategory(const FString& mobEntityTypeName) {
	const auto mobEntityType = EntityTypeFromString(std::string(TCHAR_TO_UTF8(*mobEntityTypeName)));
	if (!EntityClassTree::isTypeInstanceOf(mobEntityType, EntityType::Mob)) {
		return 0;
	}

	APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());
	return character->GetCharacterSerializeComponent()->ReadMobKillsForType(mobEntityType);
}

// D11.SSN
void UKillTrackerComponent::Client_RegisterKill_Implementation(EntityType mobEntityType, bool enchanted) {
	APlayerCharacter* character = Cast<APlayerCharacter>(GetOwner());

	character->GetClientEventHubComponent()->KilledMob(mobEntityType, enchanted);

	if (UCharacterSerializeComponent* serializer = character->GetCharacterSerializeComponent()) {
		serializer->AddMobKillForType(mobEntityType);
		if (enchanted) {
			serializer->IncrementProgressStat(EProgressStat::DEFEAT_ENCHANTED_MOBS);
		}
		if (hasMobTag(mobEntityType, MobTags::HashTag_EventMob)) {
			serializer->IncrementProgressStat(EProgressStat::DEFEAT_EVENT_MOBS);
		}
	}
}