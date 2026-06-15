#include "Dungeons.h"
#include "MerchantContext.h"
#include "type/MerchantBase.h"
#include "pricing/MerchantPricingComponent.h"
#include "currency/MerchantCurrencyComponent.h"
#include "game/component/WalletComponent.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/GameSettings.h"
#include "game/Game.h"
#include <GameFramework/Controller.h>

namespace merchant {

Context::Context(AMerchantBase& merchant, UCharacterSerializeComponent& serializer)
	: mMerchant(merchant)
	, mSerializer(serializer)
{}

const AMerchantBase& Context::GetMerchant() const {
	return mMerchant;
}

const UCharacterSerializeComponent& Context::GetSerializer() const {
	return mSerializer;
}

FName Context::GetMerchantSaveName() const {
	return mMerchant.GetSaveName();
}

bool Context::HasSaveData() const {
	return mSerializer.GetSaveData() && mSerializer.HasMechantSaveData(GetMerchantSaveName());
}

void Context::CreateSaveData() const {
	check(!HasSaveData() && "save data already created");
	mSerializer.CreateMechantSaveData(GetMerchantSaveName());
}

const FMerchantSaveData& Context::ReadSaveData() const {
	return mSerializer.ReadMechantSaveData(GetMerchantSaveName());
}

FMerchantSaveData& Context::EditSaveData() const {
	return mSerializer.EditMechantSaveData(GetMerchantSaveName());
}

int Context::GetFinishedObjectiveTagCount(const std::string& objectiveTag) const {	
	return mSerializer.GetFinishedObjectiveTagCount(objectiveTag);
}

int Context::GetProgressStat(EProgressStat stat) const {	
	return mSerializer.GetProgressStat(stat);	
}

void Context::IncrementProgressStat(EProgressStat stat) const {	
	mSerializer.IncrementProgressStat(stat);
}

}