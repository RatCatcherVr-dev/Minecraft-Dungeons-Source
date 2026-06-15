#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/avatar/AvatarInfo.h"
#include "Materials/MaterialInstance.h"
#include "game/actor/character/player/PlayerCommonTypes.h"
#include "online/entitlements/Entitlement.h"
#include "DungeonsGameInstance.h"
#include "PlayerAvatarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPortraitChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnApplyMeshMaterial, UMaterialInstance*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetPlayerColor, const FColor, color);

USTRUCT()
struct DUNGEONS_API FEntitledSkin {

	GENERATED_BODY()

	FEntitledSkin() = default;
	explicit FEntitledSkin(FName, FEntitlement);

	UPROPERTY()
	FName SkinId;

	UPROPERTY()
	FEntitlement Entitlement;
};

UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UPlayerAvatarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerAvatarComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnLocalPawnPossessed();
	
	void RefreshMeshMaterial(UMaterialInstance* pMaterial = nullptr) const;

	/**
	 * Set the player color and texture portrait for this avatar.
	 * even though the color isn't player customizable, it makes for better structured code
	 * to keep the color settings here rather than in PlayerCharacter (the parent)
	 * getters that fetch info from this component can still be implemented
	 */
	void SetPlayerColor(FColor playerColor);
	
	FColor GetPlayerColor() const { return mColor; }

	void SetPlayerColorByPlayerNumber(int playerNumber);

	FAvatarData GetPlayerAvatarData() const;

	UFUNCTION(Reliable, WithValidation, Server)
	void ServerSelectSkin(FName skinId, const FEntitlement& entitlement);

	void SelectSkin(FName skinId, const FEntitlement& entitlement);

	const FName& GetSkinId() const;

	void SetIgnoreEntitlement(bool Ignore) { IgnoreEntitlement = Ignore; }
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSetPlayerColor OnSetPlayerColor;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnPortraitChanged OnPortraitChanged;

	FOnApplyMeshMaterial OnApplyMeshMaterial;

	UPROPERTY()
	float BackgroundDarkness = 0.1f;

protected:
	void BeginPlay() override;

	UFUNCTION()
	void OnRep_SelectedSkin();
	
	UFUNCTION()
	void OnRep_Color();

	UPROPERTY()
	FColor FrameBackgroundColor;

	UPROPERTY()
	UMaterialInstance* PortraitMaterial;

private:
	bool IsOwnedByLocalPlayer() const;
	
	UDungeonsGameInstance* GetDungeonsGameInstance() const;

	void OnEntitlementsProvided(const TArray<FEntitlement>&);
	
	void DeserializeSkin(const TArray<FEntitlement>& entitlements);

	TOptional<FName> GetSerializedSkin() const;
	
	void UpdatePortrait(UMaterialInstance* pMaterial = nullptr);
	
	void SetPortraitMaterial(UMaterialInstance* material);

	FColor GetFrameBackgroundColor() const;

	uint8 CalculateFrameBackgroundColorParameter(uint8 colorParameter) const;

	bool IsValidEntitlement(FName, const FEntitlement&) const;
	
	UPROPERTY(ReplicatedUsing = OnRep_SelectedSkin)
	FEntitledSkin SelectedSkin;
	FEntitledSkin DesiredSkin;

	UPROPERTY(ReplicatedUsing = OnRep_Color)
	FColor mColor = FColor(255, 0, 255);
	TWeakObjectPtr<AAvatarInfo> mAvatarInfo;

	UPROPERTY(Transient)
	USkeletalMeshComponent* mPlayerMesh = nullptr;

	TSharedPtr<FStreamableHandle> SkinStreamingHandle;

	bool IgnoreEntitlement = false;
};
