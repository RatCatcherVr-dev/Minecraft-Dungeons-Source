#include "Dungeons.h"
#include "EyeOfEnderInstance.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/component/WalletComponent.h"
#include "game/util/UnlockKeyUtils.h"

void AEyeOfEnderInstance::Activate(const FPredictionKey& predictionKey) {
	if (HasAuthority())	{
		eyeOfEnderType = static_cast<EEyeOfEnderType>(OptionalItemDataSource->SubItemID.Get(0));
		OnRep_EyeType();
	}

	PickUpEye();
	
	Super::Activate(predictionKey);
}

void AEyeOfEnderInstance::PickUpEye_Implementation()
{
	auto* gi = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance());
	auto pcs = gi->GetUserManager()->GetAllLocalPlayerControllers();

	for (auto* pc : pcs) {
		if (auto* character = Cast<ABasePlayerController>(pc)->GetControlledPlayerCharacter())
		{
			auto* characterSerialiseComponent = character->GetCharacterSerializeComponent();
			auto strongholdData = characterSerialiseComponent->GetStrongholdData();

			if (strongholdData.AllowsPickup(eyeOfEnderType)) {
				character->GetWalletComponent()->ClientAdd(game::item::type::EyeOfEnder.getId(), 1, ECurrencyObtainReason::Pickup);
			}
			if (eyeofenderquery::isValid(eyeOfEnderType)) {
				strongholdData.GiveEye(eyeOfEnderType);
				UUnlockKeyUtils::GiveUnlockKeyToAllLocalPlayers(GetWorld(), eyeofenderquery::unlockString(eyeOfEnderType).GetValue());
			}
			characterSerialiseComponent->SetStrongholdData(strongholdData);
		}
	}
}

void AEyeOfEnderInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEyeOfEnderInstance, eyeOfEnderType)
}

void AEyeOfEnderInstance::OnRep_EyeType()
{
}
