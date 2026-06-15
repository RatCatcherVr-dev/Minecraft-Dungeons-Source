#pragma once 

#include "DungeonsWidgets/DungeonsUserWidget.h"

#include "ConnectionIconWidget.generated.h"

enum class EMinecraftAPIConnectionStatus : uint8;

USTRUCT(BlueprintType)
struct DUNGEONS_API FConnectionIconInfo {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* StatusTexture = nullptr;
};

UCLASS(Abstract)
class UConnectionIconWidget : public UDungeonsUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativePreConstruct() override;
	void Update(EMinecraftAPIConnectionStatus status);

protected:
	/* Set the connection icons in the UMG_ConnectionIcon BP */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EMinecraftAPIConnectionStatus, UTexture2D*> StatusIconMap;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UDungeonsSoftImage* StatusIcon = nullptr;
};
