#pragma once

#include "CoreMinimal.h"
#include "Net/OnlineBlueprintCallProxyBase.h"
#include "DestroyDungeonsSessionCallbackProxy.generated.h"


UCLASS()
class DUNGEONS_API UDestroyDungeonsSessionCallbackProxy : public UOnlineBlueprintCallProxyBase {
	GENERATED_UCLASS_BODY()
public:

	virtual void Activate() override;

private:

};
