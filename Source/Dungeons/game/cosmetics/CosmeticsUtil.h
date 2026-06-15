#include <Kismet/BlueprintFunctionLibrary.h>
#include "CosmeticType.h"
#include <Array.h>
#include <Text.h>
#include "CosmeticsUtil.generated.h"

class UCosmeticItemDef;

UCLASS(BlueprintType)
class UCosmeticsUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, category = "Dungeons")
	static FText getCostmeticTypeDisplayName(ECosmeticType cosmeticType);
};
