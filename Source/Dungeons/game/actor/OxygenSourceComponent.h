#pragma once

#include "GameFramework/Actor.h"
#include "OxygenSourceComponent.generated.h"

UCLASS()
class DUNGEONS_API AOxygenSourceComponent : public AActor {
	GENERATED_BODY()

public:
	AOxygenSourceComponent(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Default")
	bool GetIsEnabled() { return IsEnabled; }

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void Enable(bool enable);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void EnableSound(bool enable);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Default")
	void EnableMapPin(bool enable);

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Default")
	bool IsEnabled = false;
};