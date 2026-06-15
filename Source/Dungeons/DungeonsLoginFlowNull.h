#pragma once
#include "DungeonsLoginFlow.h"
#include "DungeonsLoginFlowNull.generated.h"


UCLASS(BlueprintType)
class UDungeonsLoginFlowNull : public UDungeonsLoginFlow
{
	GENERATED_BODY()
public:
	UDungeonsLoginFlowNull(const FObjectInitializer& ObjectInitializer);

private:
	virtual void PlatformLogin(ULocalPlayer* LocalPlayer) override;
};



