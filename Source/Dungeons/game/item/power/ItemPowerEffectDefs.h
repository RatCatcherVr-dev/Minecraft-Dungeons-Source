#pragma once

#include "ItemPowerEffect.h"
#include "ItemPowerEffectDefs.generated.h"



UCLASS(Abstract)
class DUNGEONS_API UDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UMeleeDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UMeleeDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API URangedDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	URangedDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UItemRangedDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemRangedDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UItemArtifactDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemArtifactDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UItemDamagePerSecondIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemDamagePerSecondIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UItemRangedDamagePerSecondIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemRangedDamagePerSecondIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UItemArtifactDamagePerSecondIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemArtifactDamagePerSecondIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UDamageReduction : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UDamageReduction();
};

UCLASS(Abstract)
class DUNGEONS_API UMaxHealthIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UMaxHealthIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UArmorDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UArmorDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UDurationIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UDurationIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UDamageBoosted : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UDamageBoosted();
};

UCLASS(Abstract)
class DUNGEONS_API UHealingIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UHealingIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UMeleeHealingIncrease : public UHealingIncrease {
	GENERATED_BODY()
public:
	UMeleeHealingIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API URangedHealingIncrease : public UHealingIncrease {
	GENERATED_BODY()
public:
	URangedHealingIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UArmorHealingIncrease : public UHealingIncrease {
	GENERATED_BODY()
public:
	UArmorHealingIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UStunDurationIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UStunDurationIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UPushForceIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UPushForceIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API USummonDamageIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	USummonDamageIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UCooldownReduction : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UCooldownReduction();
};

UCLASS(Abstract)
class DUNGEONS_API UItemHealthIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemHealthIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API USpeedIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	USpeedIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UItemPowerAsFlatDamageBoost : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UItemPowerAsFlatDamageBoost();
};

UCLASS(Abstract)
class DUNGEONS_API UQuiverAmmoIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UQuiverAmmoIncrease();
};

UCLASS(Abstract)
class DUNGEONS_API UDoubleItemChanceIncrease : public UItemPowerEffect {
	GENERATED_BODY()
public:
	UDoubleItemChanceIncrease();
};