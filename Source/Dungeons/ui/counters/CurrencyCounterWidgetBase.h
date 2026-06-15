// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnCounterWidgetBase.h"
#include "game/item/SerializableItemId.h"
#include "CurrencyCounterWidgetBase.generated.h"

class UWalletComponent;

UCLASS(Abstract)
class DUNGEONS_API UCurrencyCounterWidgetBase : public UPawnCounterWidgetBase
{
	GENERATED_BODY()

private:
	void OnWalletCurrencyChanged(const FSerializableItemId&);

	UPROPERTY()
	UWalletComponent* mWallet;

	bool IsInterrestedInCurrency(const FSerializableItemId&) const;
	int GetWalletBalance(const UWalletComponent&) const;
	
protected:
	TOptional<int> FetchBoundValue(const AActor&) const override;	

	void BindTo(AActor&) override;
	void UnbindFrom(AActor&) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dungeons")
	void OnCurrencyTypeChanged(const FSerializableItemId& itemId);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SetCurrencyType(const FSerializableItemId& itemId);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FSerializableItemId& GetCurrencyItemId() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsCurrencyItemId(const FSerializableItemId& itemId) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	bool HasCurrencyEntitlement(const UObject* worldContextObject) const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Dungeons")
	FSerializableItemId CurrencyItemId;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Dungeons")
	FString RequiredEntitlement;

};
