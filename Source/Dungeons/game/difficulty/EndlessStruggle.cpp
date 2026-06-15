#include "EndlessStruggle.h"
#include "AssertionMacros.h"

UEndlessStruggleLibrary::UEndlessStruggleLibrary() {
	static const auto objectFinder = ConstructorHelpers::FObjectFinder<UDataTable>(
		TEXT("DataTable'/Game/DataTables/Assets/EndlessStruggle.EndlessStruggle'"));
	EndlessStruggleTable = objectFinder.Object;
}

FEndlessStruggleRow* UEndlessStruggleLibrary::GetRow( FName name ) {
	return EndlessStruggleTable->FindRow<FEndlessStruggleRow>(name, TEXT("EndlessStruggle"));
}

FEndlessStruggleConfiguration UEndlessStruggleLibrary::GetConfiguration(int value) {
	FEndlessStruggleConfiguration config;

	auto computeFloat = [this, value]( const FName& name, float defaultValue = 1.0f ) -> float {
		auto row = EndlessStruggleTable->FindRow<FEndlessStruggleRow>(name, TEXT("EndlessStruggle"));
		if( row && value > 0 && value >= row->IntroducedAt ) {
			float minLevel = static_cast<float>(row->IntroducedAt);
			float maxLevel = static_cast<float>(NumberOfEndlessStruggleLevels);
			float fLevel = static_cast<float>(value);
			float delta = row->Curve->GetFloatValue( (fLevel - minLevel) / (maxLevel - minLevel) );
			return FMath::Lerp( row->MinValue, row->MaxValue, delta );
		}
		return defaultValue;
	};

	config.XPMultiplier = computeFloat( FName( "XPMultiplier" ), 1.0f );
	config.LootPowerMultiplier = computeFloat( FName( "LootPowerMultiplier" ), 1.0f );
	config.LootQualityBoost = computeFloat( FName( "LootQualityBoost" ), 0.0f );
	config.ShopPriceMultiplier = computeFloat( FName( "ShopPriceMultiplier" ), 1.0f );
	config.MobCountMultiplier = computeFloat( FName( "MobCountMultiplier" ), 1.0f );
	config.MobMaxHealthMultiplier = computeFloat( FName( "MobMaxHealthMultiplier" ), 1.0f );
	config.MobPerformHealingMultiplier = computeFloat( FName( "MobPerformHealingMultiplier" ), 1.0f );
	config.MobMaxEnduranceMultiplier = computeFloat( FName( "MobMaxEnduranceMultiplier" ), 1.0f );
	config.MobRecoverEnduranceMultiplier = computeFloat( FName( "MobRecoverEnduranceMultiplier" ), 1.0f );
	config.MobDamageMultiplier = computeFloat( FName( "MobDamageMultiplier" ), 1.0f );
	config.MobStunDurationMultiplier = computeFloat( FName( "MobStunDurationMultiplier" ), 1.0f );
	config.MobSpeedMultiplier = computeFloat( FName( "MobSpeedMultiplier" ), 1.0f );
	config.MobResurrectionChance = computeFloat( FName( "MobResurrectionChance" ), 0.0f );
	config.MobDeflectChance = computeFloat( FName( "MobDeflectChance" ), 0.0f );
	config.MobPushbackMultiplier = computeFloat(FName("MobPushbackMultiplier"), 1.0f);
	config.EnchantedMobChanceMultiplier = computeFloat( FName( "EnchantedMobChanceMultiplier" ), 1.0f );
	config.EnchantedMobPowerMultiplier = computeFloat( FName( "EnchantedMobPowerMultiplier" ), 1.0f );
	config.PlayerReceiveHealingMultiplier = computeFloat(FName("PlayerReceiveHealingMultiplier"), 1.0f);
	ensureAlwaysMsgf(!FMath::IsNearlyZero(config.PlayerReceiveHealingMultiplier), TEXT("PlayerReceiveHealingMultiplier cannot be 0."));

	return config;
}

const FEndlessStruggle FEndlessStruggle::ZERO = { 0 };
const FEndlessStruggle FEndlessStruggle::HIGHEST = { NumberOfEndlessStruggleLevels };
const int FEndlessStruggle::UnlockedLevelsAboveCompleted = 2;
int FEndlessStruggle::compare(const FEndlessStruggle& rhs) const {
	return Value - rhs.Value;
}
