#pragma once

#include <CoreMinimal.h>
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "ui/map/MapType.h"

#include "MapPinWidget.generated.h"

UENUM(BlueprintType)
enum class EMapPinLevel : uint8 {
	LOW,
	NORMAL,
	HIGH,
};
ENUM_NAME(EMapPinLevel);

UCLASS()
class DUNGEONS_API UMapPinWidget : public UUserWidget {
	GENERATED_BODY()

public:
	// Blueprint interactions and callbacks
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsLocalPlayer() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void DoSetAspectScale(FVector2D AspectScale);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void DoUpdatePinRarity(EItemRarity rarity, bool IsGearOrPermanent, bool isGilded);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void DoUpdateDirection(float angle);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void DoUpdateDownedState(bool downed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void DoSetPinColor(FColor color);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void DoSetPlayerCharacter(APlayerCharacter* playerCharacter);
	
	// code accessors for widget behaviour
	void InitMapPinWidget(class UMapPinComponent* parent);

	void setPinAngle(float angle);
	void setAspectScale(FVector2D AspectScale);
	void setVisible(bool visible);
	void setIsDead(bool dead);

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	bool IsRemotePlayer = false;

	UPROPERTY(BlueprintReadOnly, Category = "Dungeons")
	FColor Color;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	EOutsideBoundBehavior OutsideBoundBehavior = EOutsideBoundBehavior::RevealExplored;

private:
	bool PlayerDead = true;
	bool Initialized = false;
	bool IsVisible = true;
	float CurrentAngle = 0.0f;
	class UMapPinComponent* ParentComponent = nullptr;

};