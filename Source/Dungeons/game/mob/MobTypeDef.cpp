#include "Dungeons.h"
#include "world/entity/EntityTypes.h"
#include "util/StringUtil.h"
#include "MobTypeDef.h"

MobTypeDef::MobTypeDef(const EntityType mobType)
	: mMobType(mobType) {
}

bool MobTypeDef::IsDisabled() const {
	return bDisabled;
}

const FText& MobTypeDef::GetName() const {
	return mName;
}

const FText& MobTypeDef::GetDescriptionText() const {
	return mDescriptionText;
}

FName MobTypeDef::EntityTypeToFName(EntityType mobType) {
	return stringutil::toFName(EntityTypeToString(mobType));
}

FName MobTypeDef::GetIconRowName() const {
	if (mIconRowNameProvider) {
		return mIconRowNameProvider();
	} else {
		return EntityTypeToFName(mMobType);
	}
}

