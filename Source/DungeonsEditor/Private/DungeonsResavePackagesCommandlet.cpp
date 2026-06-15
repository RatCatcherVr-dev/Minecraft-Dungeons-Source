#include "DungeonsResavePackagesCommandlet.h"
#include "util/ClassUtil.h"
#include "GameFramework/Actor.h"

int32 UDungeonsResavePackagesCommandlet::Main(const FString& Params) {
	auto bpplayercontroller =  classutil::LoadClassAsRoot<AActor>(TEXT("/Game/Actors/Characters/Player/BP_PlayerController.BP_PlayerController_C"));

	int32 res = Super::Main(Params);

	return res;
}