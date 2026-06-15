#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AttributeUtil.h"
#include "AffectorAttributeSet.generated.h"

UCLASS()
class DUNGEONS_API UAffectorAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_DealDamageMul, Category = "Dungeons")
	float DealDamageMul = 1.f;

	DECLARE_ATTRIBUTE_FUNCTION(DealDamageMul);
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

protected:
	UFUNCTION()
	void OnRep_DealDamageMul();
};
