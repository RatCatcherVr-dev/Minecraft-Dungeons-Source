#pragma once

#include "game/component/PaperdollComponent.h"
#include "WearableCosmetic.generated.h"

UCLASS(Blueprintable)
class DUNGEONS_API AWearableCosmetic : public AActor {

	GENERATED_BODY()

public:
	AWearableCosmetic();

	void AttachToPlayerMesh(class USkeletalMeshComponent* playerMesh);

protected:
	void BeginPlay() override;	
	void EndPlay(const EEndPlayReason::Type) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPaperdollComponent* Paperdoll;
private:
	UFUNCTION()
	void OnDetachedComponentCreated(USceneComponent* component);

	UPROPERTY()
	TArray<USceneComponent*> DetachedComponents;
};