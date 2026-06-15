#pragma once

#include "CoreMinimal.h"
#include "game/cosmetics/CosmeticType.h"
#include "GameFramework/Info.h"
#include "CosmeticItemInfo.generated.h"

UCLASS(Blueprintable, BlueprintType)
class DUNGEONS_API ACosmeticItemInfo : public AInfo {
	GENERATED_BODY()
	
public:	
	ACosmeticItemInfo();

	virtual void Equip(UChildActorComponent* assignedChildActorComponent, USkeletalMeshComponent* playerMesh);	
	virtual void Unequip();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FName& GetItemName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ECosmeticType GetType() const;

	virtual TSubclassOf<AActor> GetChildActorClass();
protected:	
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dungeons")
	ECosmeticType Type;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Dungeons")
	FName ItemName;
};
