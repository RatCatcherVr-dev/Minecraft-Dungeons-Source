#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BumpableComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UBumpableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBumpableComponent();

	void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION()
	void OnActorHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
protected:

	class UCharacterMovementComponent*		m_Parent_CharacterMovementComponent;
	class UMassComponent*					m_Parent_bumplableMassComponent;

	bool HasBumpedThisTick = false;
};
