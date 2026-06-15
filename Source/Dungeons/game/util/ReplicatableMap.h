#pragma once

#include "CoreMinimal.h"
#include "ReplicatableMap.generated.h"

USTRUCT()
struct DUNGEONS_API FPair {
	GENERATED_BODY()

	FPair() = default;

	FPair(FString key, FString value);

	explicit FPair(TPair<FString, FString> pair);

	UPROPERTY()
	FString key;

	UPROPERTY()
	FString value;
};

USTRUCT()
struct DUNGEONS_API FReplicatableMap {
	GENERATED_BODY()

	FReplicatableMap() = default;

	explicit FReplicatableMap(TMap<FString, FString>& sourceMap);
	
	void Add(const FString& key, const FString& value);
	void Add(const std::string& key, const std::string& value);

	TOptional<FString> Find(const FString& key) const;

	TMap<FString, FString> ToTMap() const;

	UPROPERTY()
	TArray<FPair> pairs{};

private:
	static TArray<FPair> MakeReplicatable(TMap<FString, FString>& sourceMap);
};
