#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayerCharacterFunctionLibrary.generated.h"

class APlayerCharacter;


UCLASS()
class DUNGEONS_API UPlayerCharacterFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	
	/** Returns First Player Character with ID */
	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static APlayerCharacter* GetPlayerCharacterByPlayerId(const UObject* WorldContextObject, const int32 ID);
	
	/** Returns Number of Current Local Player Characters */
	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static int32 CountLocalPlayerCharacters(const UObject* WorldContextObject);

	/** Returns Number of Current Remote Player Characters */
	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static int32 CountRemotePlayerCharacters(const UObject* WorldContextObject);

	/** Returns First Local Player Character */
	UFUNCTION(BlueprintPure, Category = "Dungeons", meta = (WorldContext = "WorldContextObject"))
	static APlayerCharacter* GetFirstLocalPlayerCharacter(const UObject* WorldContextObject);
	
};