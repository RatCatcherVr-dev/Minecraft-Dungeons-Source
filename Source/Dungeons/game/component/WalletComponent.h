
#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemSlot.h"
#include "WalletComponent.generated.h"

class UCharacterSerializeComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWalletReplicated, UWalletComponent*, component);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWalletCountUpdated, UWalletComponent*, component);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWalletCountUpdatedInternal, const FSerializableItemId&);

UENUM(BlueprintType)
enum class ECurrencyObtainReason : uint8 {
	Default,
	Salvaged,
	Pickup
};
ENUM_NAME(ECurrencyObtainReason)

DECLARE_DELEGATE_RetVal_ThreeParams(int32, FCurrencyConsumer, const FItemId&, int32, ECurrencyObtainReason);

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UWalletComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UWalletComponent();

	void OnLocalPawnPossessed();
	
	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnWalletReplicated OnCountReplicated;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnWalletCountUpdated OnDisplayCountChanged;

	FOnWalletCountUpdatedInternal OnDisplayCountChangedInternal;

	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Dungeons")
	void ClientAdd(const FSerializableItemId& type, int32 amount, ECurrencyObtainReason reason = ECurrencyObtainReason::Default);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void Deduct(const FSerializableItemId& type, int32 amount);
	

	UFUNCTION(BlueprintCallable)
	int32 Balance(const FSerializableItemId& type) const;

	void BroadcastOnDisplayCountChanged(const FSerializableItemId& type);

	int32 GetEmeraldBalance() const;
	int32 GetGoldBalance() const;

	bool AddConsumer(const FCurrencyConsumer&);
	bool RemoveAllConsumers(const UObject*);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	mutable TArray<UItemSlot*> mCurrencySlots;
	
private:
	bool m_initialized;

	TArray<FCurrencyConsumer> Consumers;

	const UCharacterSerializeComponent* ReadOwningCharacterSerializeComponent() const;
};

