#pragma once
#include "CoreMinimal.h"
#include "MerchantDef.h"
#include "MerchantActorUtil.generated.h"

class ACharacter;
class AMerchantActor;

UCLASS()
class DUNGEONS_API UMerchantActorUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static AMerchantActor* GetFirstSelectMissionOfferingsMerchant(ACharacter* interacter, ELevelNames levelName);
};
