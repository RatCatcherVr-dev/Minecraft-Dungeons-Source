#pragma once
#include "MerchantDef.h"
#include "MerchantDefs.generated.h"

UCLASS(Const, BlueprintType)
class DUNGEONS_API UVillageMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UVillageMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API ULuxuryMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	ULuxuryMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UMysteryMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UMysteryMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UBlacksmithMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UBlacksmithMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UGiftWrapperMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UGiftWrapperMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UPiglinMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UPiglinMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UQuestGiverMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UQuestGiverMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UHyperMissionMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UHyperMissionMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UEnchantmentMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UEnchantmentMerchantDef();
};

UCLASS(Const, BlueprintType)
class DUNGEONS_API UAdventureHubMerchantDef : public UMerchantDef {
	GENERATED_BODY()
public:
	UAdventureHubMerchantDef();
};

namespace merchantdefs {
	TArray<TSubclassOf<UMerchantDef>> getAllEnabledClasses();
	TArray<TSubclassOf<UMerchantDef>> getAllEnabledMerchants();
}