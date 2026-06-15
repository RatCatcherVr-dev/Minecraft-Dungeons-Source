#pragma once

#include "CoreMinimal.h"
#include "trials/TrialsProvider.h"
#include "entitlements/EntitlementsRepository.h"
#include "MinecraftAPIAuthData.h"
#include "XAuthData.h"
#include "MinecraftAPI.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAuthenticationFailed)

UCLASS()
class DUNGEONS_API UMinecraftAPI : public UObject {
	GENERATED_BODY()

public:
	void Init();
	UWorld* GetWorld() const override;
	void Authenticate(const XAuthData&);
	void RetryAuthenticate();
	void Logout();

	UTrialsProvider* GetTrialsProvider();

	FOnAuthenticationFailed OnAuthenticationFailed;
private:
	void OnSuccessfulAuthentication(const MinecraftAPIAuthData&, const XAuthData&);
	
	void OnFailedAuthentication(const XAuthData&);

	UPROPERTY()
	UEntitlementsRepository* EntitlementsRepository;

	UPROPERTY()
	UTrialsProvider* TrialsProvider;

	FDelegateHandle OnSuccessfulAuthenticationHandle;
	
	FDelegateHandle OnFailedAuthenticationHandle;
};
