#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DebugComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDebugComponent();

	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetText(const FString& line, int lineIndex);

	void ClearText();

	TArray<FString> lines;
};
