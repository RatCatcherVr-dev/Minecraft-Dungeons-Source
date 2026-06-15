#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NoExportTypes.h"
#include "game/util/Pushback.h"
#include "DungeonsEffectLibrary.generated.h"


class UPrimitiveComponent;
/**
 * 
 */
UCLASS()
class DUNGEONS_API UDungeonsEffectLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	* Gets the material instance dynamic of the mesh
	* @param mesh - The mesh to get the material instance from.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static UMaterialInstanceDynamic* GetMaterialInstanceDynamicFromMesh(UPrimitiveComponent* mesh, int materialIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static TArray<UMaterialInstanceDynamic*> GetAllMaterialInstanceDynamicFromMesh(UPrimitiveComponent* mesh);

	/**
	* Requires the mesh to use the Model Master Material. Briefly flashes the mesh.
	* @param mesh - The mesh to flash.
	* @param color - The color to flash.
	* @param duration - The duration of the flash.
	*/
	UFUNCTION(BlueprintCallable, Category="Dungeons|Effects")
	static void HitFlashOnMesh(UPrimitiveComponent* mesh, FLinearColor color, float duration, float startFraction = 1.f, float worldTime = -1.f);

	/**
	* @See HitFlashOnMesh, Same effect but fetches mesh from the character.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void HitFlashOnCharacter(class ACharacter* character, FLinearColor color, float duration, float startFraction = 1.f, float worldTime = -1.f);

	/**
	* @See HitFlashOnMesh, Same effect but fetches mesh from the actor.
	*/
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void HitFlashOnActor(class AActor* character, FLinearColor color, float duration, float startFraction = 1.f, float worldTime = -1.f);

	/**
	* @Added to this blueprint library, should maybe go to another?
	*/
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void PushBackOnActor(const FPushback PushBack, const AActor* byWhom, const AActor* affecting);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void PushBackOnActorInDirection(const FPushback PushBack, const FVector& pushDirection, const AActor* affecting);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void JumpPadLaunchActorInDirection(const FPushback PushBack, const FVector& pushDirection, const AActor* affecting);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void AlignEffectAtActorFeet(AActor* character, UParticleSystemComponent* effect);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static FVector FindFeetOffsetForActor(AActor* actor, const float bias = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void AlignEffectToCurrentGround(AActor* actor, UParticleSystemComponent* effect, const float bias = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static FVector FindCurrentGroundOffset(AActor* actor, const float bias = 1.f);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void DissolveActor(AActor* actor, const float duration);

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Effects")
	static void AddTagToSceneComponents(AActor* actor, const FName& tag, bool includeChildActors = true);
};
