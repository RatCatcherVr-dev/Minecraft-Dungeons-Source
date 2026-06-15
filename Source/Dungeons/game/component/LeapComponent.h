#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "LeapComponent.generated.h"

UCLASS()
class UGhostLeapGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UGhostLeapGameplayEffect();
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FLeapZConfig {
	GENERATED_USTRUCT_BODY()	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|LeapConfig")
	float offsetZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|LeapConfig")
	float leapYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|LeapConfig")
	float leapAmount;
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FLeapXyConfig {
	GENERATED_USTRUCT_BODY()

	/**	Distance on the XY plane */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|LeapConfig")
	float distanceXy;

	/**	Entries with Z offsets and required yaw/amount for the leap. MUST be sorted in ascending order */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons|LeapConfig")
	TArray<FLeapZConfig> entries;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLandedDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API ULeapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULeapComponent();
	
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void HandleLanded();

	bool CanLeap();
	
	void Leap(FVector target);

	void Leap(ABaseCharacter* character, float forwardMultiplier = 0.f, float offsetRadiusMax = 0.f);

	UPROPERTY(BlueprintAssignable)
	FOnLandedDelegate OnLanded;

private:
	UPROPERTY(EditAnywhere, Category = "Dungeons|LeapComponent")
	float ghostDisableDelay = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Dungeons|LeapComponent")
	UAnimSequenceBase* leapStartSequence;

	UPROPERTY(EditAnywhere, Category = "Dungeons|LeapComponent")
	UAnimSequenceBase* landSequence;

	UPROPERTY(EditAnywhere, Category = "Dungeons|LeapComponent")
	TArray<FLeapXyConfig> entries;

	FActiveGameplayEffectHandle ghostLeapHandle;

	float landTime;

	bool isLeaping;
};
