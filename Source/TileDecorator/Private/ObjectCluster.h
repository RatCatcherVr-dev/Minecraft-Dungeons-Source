#pragma once

#include <Array.h>

class AActor;
class UClass;

namespace util { namespace actor { namespace cluster {

TArray<TArray<AActor*>> cluster(const TArray<AActor*>&, float maxDistance);
TMap<UClass*, TArray<TArray<AActor*>>> typedClusters(const TArray<AActor*>&, float maxDistance);

}}}
