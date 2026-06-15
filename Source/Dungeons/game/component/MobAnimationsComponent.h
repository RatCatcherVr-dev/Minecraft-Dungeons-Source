#pragma once

#include "Components/ActorComponent.h"
#include "MobAnimationsComponent.generated.h"

class UAnimSequenceBase;

USTRUCT()
struct FMobAnimationsData {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* BasicAttack = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* GetHitFront = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* GetHitLeft = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* GetHitRight = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* Idle = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* Novelty = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* StrongAttack = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* Walk = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* Alerted = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* TeleportIn = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* TeleportOut = nullptr;

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	FName TeleportSlot = TEXT("FullBody");

	UPROPERTY(EditAnywhere, Category = "Anim|Common")
	UAnimSequenceBase* Stunned = nullptr;

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UMobAnimationsComponent : public UActorComponent {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Anim")//, meta = (ShowOnlyInnerProperties))
	FMobAnimationsData Common;

	//void BeginPlay() override;
private:
	//void _LoadDefaults();
	//TMap<FString, TOptional<FMobAnimationsData>> typeToDataMap;
};
