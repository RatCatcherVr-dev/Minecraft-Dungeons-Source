#pragma once

#include "GameFramework/Actor.h"
#include "game/component/UnrealRespawnCandidateComponent.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "BubbleColumn.generated.h"

UCLASS()
class DUNGEONS_API ABubbleColumn : public APropActor_RepSpatializeStatic {
	GENERATED_BODY()
	
public:	
	ABubbleColumn(const FObjectInitializer& objectInitializer);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Settings")
	bool StartEnabled = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Settings")
	bool StartHot = false;

	UFUNCTION(BlueprintCallable)
	void SetOxygenEnabled(bool enabled);

	UFUNCTION(BlueprintCallable)
	void SetHot(bool hot);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Transient)
	class URespawnCandidateComponent* RespawnComponent;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_IsEnabled();
	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_IsHot();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsHot)
	bool IsHot = false;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsEnabled)
	bool IsEnabled = false;
};
