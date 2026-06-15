#pragma once

#include "CoreMinimal.h"
#include "ui/map/MapPinWidget.h"
#include "ui/map/MapType.h"
#include "MapPinComponent.generated.h"

class UMapTexture2D;

UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UMapPinComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	UMapPinComponent();

	void InitializeComponent() override;

	void Activate(bool reset) override;
	void Deactivate() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Update(EMapType MapType, APlayerCharacter* primaryPlayer);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetMapPin(UMapPinWidget* widget, EMapType MapType);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMapPinWidget* GetMapPin(EMapType MapType) const;

	// should use stored pointer to widget.
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Initialize(UMapPinWidget* Content, class UCanvasPanel* Panel, float inOpacity, FVector2D AspectScale);
	
	// Update functionality
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FVector2D Get2DPosition(const UMapTexture2D* map) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMapPinWidget* CheckRetNull() { return nullptr; }

	// getters for creating the Widget BP ----- not needed when letting component store widget pointers.
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TSubclassOf<UMapPinWidget> getMapPinClass(EMapType MapType) const;

	// should use stored pointer to widget.
	void addToCanvas(UWidget* Content, class UCanvasPanel* Panel);
	void setCenterPos(const FVector& center);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	static const float PIN_ROTATION_OFFSET;

	void AddMapPinClass(EMapType, TSubclassOf<UMapPinWidget>);

	bool HasAnyMapPinClasses()const { return mapPinClasses.Num() > 0; };

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TMap<EMapType, TSubclassOf<UMapPinWidget>> mapPinClasses;

	UPROPERTY(VisibleInstanceOnly, Category = "Dungeons")
	TArray<TWeakObjectPtr<UMapPinWidget>> mapPinWidgets;

private:
	FVector PlayerCenter;
	TWeakObjectPtr<UMapPinWidget> PinWidget;
	
	void UpdateLocalCoopColours(int32 playerIndex = -1);

	FDelegateHandle OnPlayerAddedDelegateHandle;
	FDelegateHandle OnPlayerRemovedDelegateHandle;

	float LevelThreshhold = 2500.0f;
};
