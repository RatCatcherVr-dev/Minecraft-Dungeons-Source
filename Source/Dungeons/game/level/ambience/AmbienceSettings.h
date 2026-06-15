#pragma once

#include "CoreMinimal.h"
#include "AmbienceSettings.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FSkylightSettings {
	GENERATED_BODY()
public:

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Vissibility;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Intensity;

		UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor Color;
		
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FDirectionalLightSettings {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Visibility;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Intensity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FLinearColor Color;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Angle;

};

USTRUCT(BlueprintType)
struct DUNGEONS_API FExponentialHeightFogSettings {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool Visibility;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Density;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor Color;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Falloff;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector Location;
	
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FWorldLightSettings {
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool UpdateSkylight;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "UpdateSkylight"))
	FSkylightSettings SkylightSettings;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool UpdateDirectionalLight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "UpdateDirectionalLight"))
	FDirectionalLightSettings DirectionalLightSettings;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool UpdateFog;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition = "UpdateFog"))
	FExponentialHeightFogSettings ExponentialHeightFogSettings;
	
};

USTRUCT(BlueprintType)
struct DUNGEONS_API FAmbienceSettings {
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UpdateWorldLight;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (EditCondition =  "UpdateWorldLight"))
	FWorldLightSettings WorldLightSettings;
};

