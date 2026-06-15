#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Array.h"
#include "Components/SceneComponent.h"
#include "RefreshSuppliesComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUsedChangedSignature, const TArray<APlayerCharacter*>&, usedByPlayers);

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API URefreshSuppliesComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URefreshSuppliesComponent();

	void TryDropSuppliesFor(APlayerCharacter* instigator);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnUsedChangedSignature OnUsedByChanged;
private:
	UPROPERTY(Replicated, ReplicatedUsing=OnRep_UsedByChanged)
	TArray<APlayerCharacter*> UsedBy;

	UFUNCTION()
	void OnRep_UsedByChanged();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const;

};
