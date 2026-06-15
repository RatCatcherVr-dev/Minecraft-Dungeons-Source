#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "PaperdollComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAttachedToPlayerMesh, USceneComponent*);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DUNGEONS_API UPaperdollComponent : public USkeletalMeshComponent {
	
	GENERATED_BODY()

public:	
	UPaperdollComponent();

	void AttachToPlayerMesh(USkeletalMeshComponent* playerMesh);

	FOnAttachedToPlayerMesh OnAttachedToPlayerMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	bool ComponentsCanUsePhysics = false;


private:
	TArray<USceneComponent*> GetChildrenComponents() const;
};