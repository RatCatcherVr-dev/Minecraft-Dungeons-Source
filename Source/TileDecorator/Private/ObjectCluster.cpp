#include "TileDecorator.h"
#include "ObjectCluster.h"
#include <GameFramework/Actor.h>

namespace util { namespace actor { namespace cluster {

int clusterIndex(const TArray<TArray<AActor*>>& clusters, FVector p, float maxSquaredDistance) {
	for (int i = 0; i < clusters.Num(); ++i) {
		const auto& cluster = clusters[i];
		if (FVector::DistSquared(cluster.Last()->GetActorLocation(), p) <= maxSquaredDistance) {
			return i;
		}
	}
	return INDEX_NONE;
}

void cluster(TArray<TArray<AActor*>>& clusters, AActor* object, float maxSquaredDistance) {
	const auto index = clusterIndex(clusters, object->GetActorLocation(), maxSquaredDistance);
	if (index >= 0) {
		clusters[index].Add(object);
	} else {
		clusters.Add({ object });
	}
}

TArray<TArray<AActor*>> cluster(const TArray<AActor*>& objects, float maxDistance) {
	TArray<TArray<AActor*>> clusters;
	const float maxSquaredDistance = maxDistance * maxDistance;

	for (auto object : objects) {
		cluster(clusters, object, maxSquaredDistance);
	}
	return clusters;
}

TMap<UClass*, TArray<TArray<AActor*>>> typedClusters(const TArray<AActor*>& objects, float maxDistance) {
	TMap<UClass*, TArray<TArray<AActor*>>> map;
	const float maxSquaredDistance = maxDistance * maxDistance;

	for (auto object : objects) {
		cluster(map.FindOrAdd(object->GetClass()), object, maxSquaredDistance);
	}
	return map;
}

}}}
