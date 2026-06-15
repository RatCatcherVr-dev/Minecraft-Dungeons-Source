#pragma once

#include "game/component/CharacterSerializeComponent.h"
#include "interfaces/PlayerCharacterDataProvider.h"
#include "game/actor/character/BaseCharacter.h"
#include "PlayerCharacterSaveSlot.generated.h"

class UEquipmentComponent;
class UPlayerAvatarComponent;
class UCharacterSaveData;
class UCosmeticsComponent;
class UCosmeticsSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveSlotDataChanged);

UCLASS(BlueprintType)
class DUNGEONS_API APlayerCharacterSaveSlot : public ABaseCharacter, public IPlayerCharacterDataProvider {
	GENERATED_BODY()
public:

	APlayerCharacterSaveSlot(const FObjectInitializer& ObjectInitializer);

	void AssignSaveData(UCharacterSaveData* data) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ChangeSkinId(FName skinId);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ChangeName(FString Name);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void ChangeLegendaryStatus(ELegendaryStatus status);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ELegendaryStatus GetLegendaryStatus();
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UEquipmentComponent* GetEquipmentComponent() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UPlayerAvatarComponent* GetPlayerAvatarComponent() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UCosmeticsComponent* GetCosmeticsComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDLCRequired();
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsDLCEntitled();

	void StripDLCNotEntitled();

	int32 GetLevel_Implementation() const override;
	int32 GetTotalEquippedGearPower_Implementation() const override;
	int32 GetOwnedEmeralds_Implementation() const override;
	int32 GetOwnedGold_Implementation() const override;
	FText GetCharacterName_Implementation() const override;
	FName GetCharacterSkinId_Implementation() const override;
	
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FDateTime GetDateTime() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FDateTime GetCloudUploadeDateTime() const;
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FGuid GetCloudPlayerId();
	void SetCloudUploadeDateTime(FDateTime DateTime);
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void UpdateDateTimeToNow();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UItemStashComponent* GetItemStashComponent() const;

	bool IsLocallyControlled() const override;
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnSaveSlotDataChanged OnSaveSlotDataChanged;


	UCharacterSerializeComponent* GetCharacterSerializeComponent();

	void IgnoreEntitlements();
private:
	UPROPERTY(Transient)
	UItemStashComponent* ItemStashComponent;

	UPROPERTY(Transient)
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(Transient)
	UPlayerAvatarComponent* PlayerAvatarComponent;

	UPROPERTY(Transient)
	UCosmeticsComponent* CosmeticsComponent;

	UPROPERTY(Transient)
	UCharacterSerializeComponent* CharacterSerializeComponent;

	UPROPERTY(Transient)
	UCosmeticsSlot* CosmeticCapeSlot;

	UPROPERTY(Transient)
	UCosmeticsSlot* CosmeticPetSlot;

	FDateTime mCloudUploadeDateTime;

	TArray<class UCosmeticItemDef*>  GetCosmeticDefinitions();

};
