#pragma once

#include "CoreMinimal.h"
#include "game/item/InventoryItemData.h"
#include "Components/SceneComponent.h"
#include "Internationalization/Text.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHighlightBegin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHighlightEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNoMoreInteractors);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, ACharacter*, Instigator);

UENUM(BlueprintType)
enum class EClickyEnum : uint8 {
	CE_MapTable			UMETA(DisplayName = "Map table"),
	CE_Inventory		UMETA(DisplayName = "Inventory"),
	CE_LobbyChest		UMETA(DisplayName = "LobbyChest"),
	CE_Interactable		UMETA(DisplayName = "Interactable"),
	CE_Item				UMETA(DisplayName = "Item"),
	CE_IceBlock			UMETA(DisplayName = "IceBlock"),
	CE_Merchant			UMETA(DisplayName = "Merchant"),
	CE_Disabled			UMETA(DisplayName = "Disabled")
};

UENUM(BlueprintType)
enum class EBalloonTipBehavior : uint8 {
	AlwaysVisible		UMETA(DisplayName = "Always visible"),
	VisibleOnHover		UMETA(DisplayName = "Visible on hover"),
	Hidden				UMETA(DisplayName = "Hidden")
};

UENUM(BlueprintType)
enum class EBalloonItemDescriptionBehavior : uint8 {
	Detailed		UMETA(DisplayName = "Full detailed view"),
	Collapsed		UMETA(DisplayName = "Minimal view")
};

// #D11.CM
USTRUCT(BlueprintType)
struct DUNGEONS_API FContextSensitiveAction 
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FText ActionText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FName Action { "MainAttackGamepad" };
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UInteractableComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	static TMap< AActor*, UInteractableComponent* > s_ActorToInteractableComponentMap;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EClickyEnum GetType() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetBalloontipText() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetBalloonTipDescription() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EBalloonTipBehavior GetBalloontipBehavior() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FInventoryItemData& GetBalloonItemData() const;

	UFUNCTION(Category = "Dungeons")
	void EnsureWorldHud();

	UFUNCTION(Category = "Dungeons")
	void RemoveWorldHud();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsHighlighted() const;

	void SetType(EClickyEnum clickyType);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetBalloontipBehavior(EBalloonTipBehavior behaviour);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetText(const FText& text);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetTextDesc(const FText& text, const FText& description);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	void BeginHighlight(APlayerCharacter* interactingPlayer);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Dungeons")
	void EndHighlight(APlayerCharacter* interactingPlayer);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnHighlightBegin OnHighlightBegin;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnHighlightEnd OnHighlightEnd;
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnNoMoreInteractors OnNoMoreInteractors;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnInteracted OnInteracted;


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static EBalloonItemDescriptionBehavior GetBalloonItemDescriptionBehavior(const FInventoryItemData& balloonItemData);

	UFUNCTION()
	float GetRadius() const;

	UFUNCTION()
	float GetRadiusGamepadModifier() const; // D11.DB - Allow gamepad specific adjustments to interaction radii.

	virtual void Interacted(ACharacter* instigator);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void DisableInteraction();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual void EnableInteraction();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsInteractionEnalbed() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool ShouldShowObjectiveMarker() const { return bInteractionEnabled || bAlwaysShowObjective; } ;

	static UInteractableComponent* GetComponentFromActor(const AActor* pActor);

	AActor* GetCachedOwner() { return CachedOwner.Get(); };

	// #D11.CM - Context Sensitive Actions
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FContextSensitiveAction GetContextSensitiveAction() const { return ActionInformation; }

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetContextSensitiveAction(FContextSensitiveAction action) { ActionInformation = action; }

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void PurgeAllInteractors();

 	// #D11.CM
 	UFUNCTION(BlueprintCallable, Category = "Dungeons")
 	TArray<APlayerCharacter*> GetInteractingPlayers() { return CachedInteractingPlayers; };

	// #D11.CM
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasBeenUsedBy(APlayerCharacter* interactor) { return InteractedPlayers.Contains(interactor); };

	bool constantHighlight = false;

	int GetHighlightCount() const { return HighlightCount; };


	UFUNCTION()
	void OnInteractingPlayerDestroyed(AActor* actor);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FVector2D GetContextPromptOffset() const { return ContextPromptScreenspaceOffset; };

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetContextPromptOffset(FVector2D offset) { ContextPromptScreenspaceOffset = offset; };

	UPROPERTY(EditAnywhere)
	bool bOneInteractionPerPlayer = false;

	UPROPERTY(EditAnywhere)
	bool bOneTimeInteraction = false;
private:
	bool DoesPlatformWantClickyWorldspaceHud() const;

	UPROPERTY(EditAnywhere, Category = "Dungeons", meta = (EditCondition = "Enabled"))
	EClickyEnum Type;

	UPROPERTY(EditAnywhere, Category = "Dungeons", meta = (EditCondition = "Enabled"))
	FText BalloontipText;

	UPROPERTY(EditAnywhere, Category = "Dungeons", meta = (EditCondition = "Enabled"))
	FText BalloontipDescription;

	UPROPERTY(EditAnywhere, Category = "Dungeons", meta = (EditCondition = "Enabled"))
	EBalloonTipBehavior BalloonTipBehavior;

	UPROPERTY(EditAnywhere, Category = "Dungeons", meta = (EditCondition = "Enabled"))
	float Radius = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons", meta = (EditCondition = "Enabled"))
	float RadiusGamepadModifier = 0.0f; // D11.DB

	// #D11.CM
	UPROPERTY(EditAnywhere, Category = "Dungeons/Context Sensitive", meta = (EditCondition = "Enabled"))
	FContextSensitiveAction ActionInformation;

	// #D11.CM - (-47, 80) works with most regular interactions
	UPROPERTY(EditAnywhere, Category = "Dungeons/Context Sensitive", meta = (EditCondition = "Enabled"))
	FVector2D ContextPromptScreenspaceOffset { -47, 80 };

	TWeakObjectPtr<AActor> CachedOwner;

	UPROPERTY()
	TArray<APlayerCharacter*> CachedInteractingPlayers;

	UPROPERTY()
	TArray<UPrimitiveComponent*> HighlightedComponents;

	bool bAddedToWorldHud;

	int HighlightCount = 0;

	UPROPERTY(EditDefaultsOnly)
	bool bInteractionEnabled = true;

	// #D11.CM - If this is an objective interact, should we show up even when interaction isn't enabled?
	UPROPERTY(EditDefaultsOnly)
	bool bAlwaysShowObjective = false;

	TArray<APlayerCharacter*> InteractedPlayers;

};
