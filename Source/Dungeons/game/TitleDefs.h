#pragma once
#include "CoreMinimal.h"
#include "game/UniqueId.h"
#include "TitleDefs.generated.h"

UENUM(BlueprintType)
enum class ETitle : uint8 {
	INVALID,
	NONE,
	BOOMER,
	CREEPERS_BANE,
	END
};
ENUM_NAME(ETitle);
enum_bitwise_operators(ETitle, int);

class UnlockableTitle {
public:
	UnlockableTitle(const ETitle& id, const FText& title, const FText& description);

	const ETitle& GetID() const;
	const FText& GetTitle() const;
	const FText& GetDescription() const;
private:
	ETitle mId;
	FText mTitle;
	FText mDescription;
};

USTRUCT(BlueprintType)
struct FBPUnlockableTitle {
	GENERATED_USTRUCT_BODY()

	FBPUnlockableTitle();
	FBPUnlockableTitle(const ETitle& id, const FText& title, const FText& description);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UnlockableTitles")
	ETitle mId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UnlockableTitles")
	FText mTitle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UnlockableTitles")
	FText mDescription;
};

template class DUNGEONS_API TIdRegistry<ETitle, const UnlockableTitle>;
using FTitleRegistry = TIdRegistry<ETitle, const UnlockableTitle>;
using FTitleId = FTitleRegistry::TId;
DUNGEONS_API FTitleRegistry& GetTitleRegistry();

UCLASS()
class UTitlesBlueprintLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "UnlockableTitles")
	static TArray<FBPUnlockableTitle> GetAllTitles();
	UFUNCTION(BlueprintCallable, Category = "UnlockableTitles")
	static FText GetTitle(ETitle title);
	UFUNCTION(BlueprintCallable, Category = "UnlockableTitles")
	static FText GetDescription(ETitle title);
};

namespace game {
	namespace title {
		// tracking implemented
		extern const UnlockableTitle& None;
		extern const UnlockableTitle& Boomer;
		extern const UnlockableTitle& CreepersBane;
		// tracking unimplemented
	}
}