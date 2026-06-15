#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Animation/AnimSequenceBase.h>
#include "TurnComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UTurnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTurnComponent();

	void Turn(float _targetAngle, float _speed = 180.f);

	bool IsTurning() const;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static float WrapAngle(float angle);

	static float GetWinding(float a, float b);

private:
	float targetAngle = 0.f;
		
	float speed = 180.f;
	
	float sign = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	class UAnimSequenceBase* sequence = nullptr;
};
