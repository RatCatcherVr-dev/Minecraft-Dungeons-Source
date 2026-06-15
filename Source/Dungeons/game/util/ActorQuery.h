#pragma once

#include <Runtime/Engine/Public/EngineUtils.h>
#include <Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h>
#include "CommonTypes.h"
#include "world/level/BlockSource.h"
//D11.PS
#include "ActorQuery.generated.h"

enum class EntityType : unsigned int;
class ABaseCharacter;
class AMobCharacter;
class UPrimitiveComponent;
class AGameBP;
class UHealthComponent;
class UCapsuleComponent;

namespace game { class Game; }
namespace affector { class Affectors; }

enum class ECustomTraceChannels {
	IgnoreTerrain = ECollisionChannel::ECC_GameTraceChannel1,
	FromMob = ECollisionChannel::ECC_GameTraceChannel2,
	IgnorePlayer = ECollisionChannel::ECC_GameTraceChannel3,
	TerrainAndPlayerOnly = ECollisionChannel::ECC_GameTraceChannel4,
	PlayerPlane = ECollisionChannel::ECC_GameTraceChannel5,
	ArrowPlane = ECollisionChannel::ECC_GameTraceChannel6,
	NearMissTargetSelect = ECollisionChannel::ECC_GameTraceChannel7,
	TargetSelect = ECollisionChannel::ECC_GameTraceChannel8,
	Projectile = ECollisionChannel::ECC_GameTraceChannel9,
	TerrainOnly = ECollisionChannel::ECC_GameTraceChannel10,
	PlayerPawn = ECollisionChannel::ECC_GameTraceChannel11,
	KillzoneBlocker = ECollisionChannel::ECC_GameTraceChannel12,
	MobWall = ECollisionChannel::ECC_GameTraceChannel13,
	PushObject = ECollisionChannel::ECC_GameTraceChannel14,
	PushReceiver = ECollisionChannel::ECC_GameTraceChannel15,
};

struct DUNGEONS_API FActorsWithAngle
{
	TArray<AActor*> actors;
	float actorAngle = 0;
};

UCLASS()
class DUNGEONS_API UActorQuery : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery")
	static float GetActorDistance(const AActor* a, const AActor* b);

	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery")
	static float GetActorDistanceSquared(const AActor* a, const AActor* b);

	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery")
	static bool GetClosestSocket(const AActor* actor, const AActor* targetActor, FName& socketName, FVector& location);	
	
	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery")
	static FVector GetActorHealthBarPosition(const APlayerController* viewingPlayer, const AActor* actor);

	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery")
	static AGameBP* GetGameBP(const UObject* WorldContextObject);

	/**
	 * Returns True if the thisActor is within DistanceCheck range of any of the current PlayerCharacter actors
	 */
	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool IsInRangeOfPlayerCharacter(const UObject* WorldContextObject, const AActor* thisActor, const float DistanceCheck);

	/**
	 * Returns True if the location is within DistanceCheck range of any of the current PlayerCharacter actors
	 */
	UFUNCTION(BlueprintPure, Category = "Dungeons|ActorQuery", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool IsLocationRangeOfPlayerCharacter(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck);

	/**
	 * Returns True if the any player characters are in range of DistanceCheck
	 */	
	UFUNCTION(BlueprintCallable, Category = "Dungeons|ActorQuery", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool GetPlayerCharactersInRange(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< APlayerCharacter* >& PlayerList, bool OrderByDistance = false);
	static bool GetPlayerCharactersInRange(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< TWeakObjectPtr<APlayerCharacter> >& PlayerList, bool OrderByDistance = false);
	static bool GetPlayerCharactersInRangePred(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< APlayerCharacter* >& PlayerList, bool OrderByDistance = false, std::function< bool(APlayerCharacter*) > PredFunc = [](APlayerCharacter*) {return true; });
	static bool GetPlayerCharactersInRangePred(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< TWeakObjectPtr<APlayerCharacter> >& PlayerList, bool OrderByDistance = false, std::function< bool(APlayerCharacter*) > PredFunc = [](APlayerCharacter*) {return true; });

	UFUNCTION(BlueprintCallable, Category = "Dungeons|ActorQuery", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static bool GetFriendlyMobCharactersInRange(const UObject* WorldContextObject, const ABaseCharacter* ActorReference, const float DistanceCheck, TArray< AMobCharacter* >& MobList, bool OrderByDistance = false);
};

namespace actorquery {

bool isActorTerrain(AActor* actor);

inline bool isActorTerrain(TWeakObjectPtr<AActor> actor) { return actor.Get() != nullptr && isActorTerrain(actor.Get()); }

bool isActorTargetableByPlayer(AActor* actor);

inline bool isActorTargetableByPlayer(TWeakObjectPtr<AActor> actor) { return actor.Get() != nullptr && isActorTargetableByPlayer(actor.Get()); }

float getCapsuleDistance2D(const UCapsuleComponent* a, const UCapsuleComponent* b);

DUNGEONS_API float getCapsuleDistance2D(const AActor* a, const AActor* b);

float getActorDistance(const AActor* a, const AActor* b);

float getActorDistance(const FVector &sourcePoint, const AActor* actor);

float getActorCapsuleRadius(const AActor*);

float getCapsuleDistance2DSquared(const AActor* a, const AActor* b);

DUNGEONS_API float getActorDistanceSquared(const AActor* a, const AActor* b);

float getActorDistanceSquared(const FVector &sourcePoint, const AActor* actor);

float getActorDistance2D(const AActor* a, const AActor* b);

float getActorDistanceSquared2D(const AActor* a, const AActor* b);

AActor* getFurthestPlayer(const AActor* source, float radius, float& distanceSquaredOut, std::function<bool(AActor*)> filter = [](AActor*) { return true; });

AActor* getFurthestMob(const AActor* source, float radius, float& distanceSquaredOut, std::function<bool(AActor*)> filter = [](AActor*) { return true; });

AActor* getFurthestActor(const AActor* source, float radius, const TSubclassOf<class AActor>& actorType, float& distance, bool checkIfAlive = false, std::function<bool(AActor*)> filter = [](AActor*) { return true; });

template <class T>
AActor* getFurthestFromLocation(const FVector& location, float radius, float& distance_squared_out, UWorld* world, std::function<bool(AActor*)> filter = [](AActor*) { return true; }, int sourceActorID = INDEX_NONE, bool includeInvisiblePlayers = false);

AActor* getClosestPlayer(const AActor* source, float radius, float& distanceSquaredOut, std::function<bool(AActor*)> filter = [](AActor*) { return true; });

AActor* getClosestMob(const AActor* source, float radius, float& distanceSquaredOut, std::function<bool(AActor*)> filter = [](AActor*) { return true; });

DUNGEONS_API AActor* getClosestActor(const AActor* source, float radius, const TSubclassOf<class AActor>& actorType, float& distance, bool checkIfAlive = false, std::function<bool(AActor*)> filter = [](AActor*) { return true; });
DUNGEONS_API AActor* getClosestActorFromArray(const AActor* source, const TArray<AActor*> actorArray);

template <class T>
AActor* getClosestToLocation(const FVector& location, float radius, float& distance_squared_out, UWorld* world, std::function<bool(AActor*)> filter = [](AActor*) { return true; }, int sourceActorID = INDEX_NONE, bool includeInvisiblePlayers = false);

template <class T>
TArray<T*> getNearbyActors(const AActor* source, float radius, Pred<T*> pred = [](T*) { return true; }) {
	check(IsInGameThread());
	const float radius_sq = radius * radius;
	TArray<T*> actors;
	for (auto&& actor : TActorRange<T>(source->GetWorld())) {
		if (actor == source || !pred(actor))
			continue;
		if (getActorDistanceSquared(source, actor) <= radius_sq)
			actors.Add(actor);
	}
	return actors;
}

template <class T>
TArray<T*> getNearbyActors(UWorld* world, const FVector &sourcePoint, float radius, Pred<T*> pred = [](T*) { return true; }) {
	check(IsInGameThread());
	const float radius_sq = radius * radius;
	TArray<T*> actors;
	for (auto&& actor : TActorRange<T>(world)) {
		if (!pred(actor))
			continue;
		if (getActorDistanceSquared(sourcePoint, actor) <= radius_sq)
			actors.Add(actor);
	}
	return actors;
}


template <class T>
TArray<TTuple<T*, float>> getNearbyInstanceTrackedActorsDistanceSquared(const AActor* source, float radius, Pred<T*> pred = [](T*) { return true; }) {
	ensure(source);
	const float radius_sq = radius * radius;
	TArray<TTuple<T*, float>> result;
	const FVector sourcePoint = source->GetActorLocation();
	for (auto&& actor : InstanceTracker<T>::GetList(source->GetWorld())) {
		if (actor == source || !pred(actor))
			continue;
		float dist = getActorDistanceSquared(sourcePoint, actor);
		if (dist <= radius_sq) {
			result.Emplace(actor, dist);
		}
	}
	return result;
}


template <class T>
TArray<T*> getNearbyInstanceTrackedActors(const AActor* source, float radius, Pred<T*> pred = [](T*) { return true; }) {
	const float radius_sq = radius * radius;
	TArray<T*> actors;
	for (auto&& actor : InstanceTracker<T>::GetList(source->GetWorld())) {
		if (actor == source || !pred(actor))
			continue;
		if (getActorDistanceSquared(source, actor) <= radius_sq)
			actors.Add(actor);
	}
	return actors;
}


template <class T>
TArray<T*> getNearbyInstanceTrackedActors(UWorld* world, const FVector &sourcePoint, float radius, Pred<T*> pred = [](T*) { return true; }) {
	const float radius_sq = radius * radius;
	TArray<T*> actors;
	for (auto&& actor : InstanceTracker<T>::GetList(world)) {
		if (!pred(actor))
			continue;
		if (getActorDistanceSquared(sourcePoint, actor) <= radius_sq)
			actors.Add(actor);
	}
	return actors;
}

template <class T>
TArray<T*> getInstanceTrackedActors(UWorld* world, Pred<T*> pred = [](const T*) { return true; }) {
	TArray<T*> actors;
	for (auto&& actor : InstanceTracker<T>::GetList(world)) {
		if (pred(actor)){
			actors.Add(actor);
		}
	}
	return actors;
}

DUNGEONS_API AActor* getClosestActorInCone2D(const AActor* source, const TSubclassOf<AActor>& actorType, float maxDist, float maxAngle, bool traceHit, Pred<const AActor*> pred);
FActorsWithAngle getActorsInCone2DWithAngle(const AActor* source, FVector sourceOffset, FVector direction, const TSubclassOf<AActor>& actorType, float maxDistUnits, float maxAngleRadians, Pred<const AActor*> pred, bool bDebug);

TArray<AActor*> getActorsInCone2D(const AActor* source, const TSubclassOf<AActor>& actorType, float maxDist, float maxAngle, Pred<const AActor*> pred);

void getActorsInCylinder(UWorld* world, FVector fromLocation, FVector cylinderDirection, const TSubclassOf<AActor>& actorType, float cylinderLength, float radius, float cylinderOffset, TArray<AActor*>& list);

bool isActorInExtraMargin(const AActor& actor, FVector source, FVector direction, float maxDist, float maxAngleRadians, float maxZDiff);

bool isInArc(FVector source, FVector normalizedDirection, FVector location, float maxHalfAngleRadians);

bool isActorInWedge(FVector source, FVector normalizedDirection, AActor* actor, float minDistUnits, float maxDistUnits, float maxHalfAngleRadians, float maxZDiff);

template <class T>
void getActorsInWedge(UWorld* world, FVector source, FVector direction, float minDistUnits, float maxDistUnits, float maxHalfAngleRadians, float maxZDiff, TArray<T*>& list, bool OnlyTickingActors = false) {
	direction.Normalize();

	if (OnlyTickingActors)
	{
		for (auto&& actor : TActorRange<T>(world)) {
			if(!actor->IsActorTickEnabled())
				continue;
			//D11.PS
			auto health = actor->template FindComponentByClass<UHealthComponent>();
			if (health != nullptr && health->IsNotAlive()) {
				continue;
			}

			if (isActorInWedge(source, direction, actor, minDistUnits, maxDistUnits, maxHalfAngleRadians, maxZDiff)) {
				list.Add(actor);
			}
		}
	}
	else
	{
		for (auto&& actor : TActorRange<T>(world)) {
			//D11.PS
			auto health = actor->template FindComponentByClass<UHealthComponent>();
			if (health != nullptr && health->IsNotAlive()) {
				continue;
			}

			if (isActorInWedge(source, direction, actor, minDistUnits, maxDistUnits, maxHalfAngleRadians, maxZDiff)) {
				list.Add(actor);
			}
		}
	}
}

template <class T>
void getInstanceTrackedActorsInWedge(UWorld* world, FVector source, FVector direction, float minDistUnits, float maxDistUnits, float maxHalfAngleRadians, float maxZDiff, TArray<T*>& list, bool OnlyTickingActors = false) {
	direction.Normalize();

	if (OnlyTickingActors)
	{
		for (auto&& actor : InstanceTracker<T>::GetList(world)) {
			if (!actor->IsActorTickEnabled())
				continue;
			//D11.PS
			auto health = actor->template FindComponentByClass<UHealthComponent>();
			if (health != nullptr && health->IsNotAlive()) {
				continue;
			}

			if (isActorInWedge(source, direction, actor, minDistUnits, maxDistUnits, maxHalfAngleRadians, maxZDiff)) {
				list.Add(actor);
			}
		}
	}
	else
	{
		for (auto&& actor : InstanceTracker<T>::GetList(world)) {
			//D11.PS
			auto health = actor->template FindComponentByClass<UHealthComponent>();
			if (health != nullptr && health->IsNotAlive()) {
				continue;
			}

			if (isActorInWedge(source, direction, actor, minDistUnits, maxDistUnits, maxHalfAngleRadians, maxZDiff)) {
				list.Add(actor);
			}
		}
	}
}

bool getClosestBoneFromList(const FVector& closestToLocation, const USkeletalMeshComponent* mesh, const TArray<FName>& boneList, FName& socketName, FVector& location);

bool getClosestSocket(const FVector& closestToLocation, const AActor* targetActor, FName& socketName, FVector& location);

bool getClosestBone(const FVector& closestToLocation, const USkeletalMeshComponent* mesh, FName& boneName, FVector& location);

bool getClosestBone(const FVector& closestToLocation, const AActor* targetActor, FName& boneName, FVector& location);

bool getClosestBoneOnPhysicsAsset(const FVector& closestToLocation, const AActor* targetActor, FName& boneName, FVector& location, bool approximate);

FVector getActorHealthBarPosition(const APlayerController* viewingPlayer, const AActor* actor);

template <typename ActorType, typename Filter, typename Scorer>
ActorType* getActor(
	UWorld* world,
	Filter filter,
	Scorer getScore
) {
	ActorType* bestActor { nullptr };
	float bestScore { std::numeric_limits<float>::infinity() };
	check(IsInGameThread());
	for (auto&& candidateActor : TActorRange<ActorType>(world)) {
		if (!filter(candidateActor)) {
			continue;
		}

		const auto candidateScore = getScore(candidateActor);
		if (candidateScore < bestScore) {
			bestActor = candidateActor;
			bestScore = candidateScore;
		}
	}

	return bestActor;
}

template <typename ActorType, typename Filter, typename Scorer>
ActorType* getActor(
	UWorld* world,
	Filter filter,
	Scorer getScore,
	const TArray<ActorType*>& actors
) {
	ActorType* bestActor{ nullptr };
	float bestScore{ std::numeric_limits<float>::infinity() };
	check(IsInGameThread());
	for (auto&& candidateActor : actors) {
		if (!filter(candidateActor)) {
			continue;
		}

		const auto candidateScore = getScore(candidateActor);
		if (candidateScore < bestScore) {
			bestActor = candidateActor;
			bestScore = candidateScore;
		}
	}

	return bestActor;
}

template <typename ActorType>
TArray<ActorType*> getActors(UWorld* world) {
	TArray<ActorType*> actors;
	check(IsInGameThread());
	for (auto&& candidate : TActorRange<ActorType>(world)) {		
		actors.Add(candidate);		
	}

	return actors;
}

template <typename ActorType, typename Filter>
TArray<ActorType*> getActors(UWorld* world, const Filter& filter) {
	TArray<ActorType*> actors;
	check(IsInGameThread());
	for (auto&& candidate : TActorRange<ActorType>(world)) {
		if (filter(candidate)) {
			actors.Add(candidate);
		}
	}

	return actors;
}

template <typename ActorType, typename Filter>
int countActors(UWorld* world, const Filter& filter) {
	int count { 0 };
	
	for (auto&& candidate : InstanceTracker<ActorType>::GetList(world)) {
		if (filter(candidate)) {
			count++;
		}
	}

	return count;
}

AActor* getClosestPlayer(AActor* source);

AActor* getWeakestPlayer(AActor* source);

AActor* getClosestMob(AActor* source);

AActor* getWeakestMob(AActor* source);

AActor* getWeakestCharacterAmong(UWorld* world, const TArray<ABaseCharacter*>& characters);

namespace is {
	bool alive(const AActor*);
	bool alive(const ABaseCharacter*);
	bool aiVisible(const AActor*);
	bool aliveAndAiVisible(const AActor*);
	bool neutral(const AActor*); // @note: where's the definition?
	bool healthMaxed(const AActor*);

	DUNGEONS_API       Pred<const AActor*>  inRange(const AActor*, float radius);
	                   Pred<const AActor*>  inRange(FVector, float radius);
	                   Pred<const AActor*>  entityType(EntityType);
	             const Pred<const AActor*>& player();
	             const Pred<const AActor*>& mob();
	             const Pred<const AActor*>& mobWithOnScreenHealthbar();
				 const Pred<const AActor*>& mobShouldOverrideMusicTrack();
	             const Pred<const AActor*> tagged(const FName Tag);
}

namespace min {
	std::function<float(const AActor*)> distance(const AActor*);
	float health(const AActor*);
}

bool isAlive(const AActor*);

template <class ComponentType, class ActorType>
ComponentType* getFirstComponentByActorType(UWorld* world) {
	for (auto&& actor : TActorRange<ActorType>(world)) {
		if (auto component = actor->template FindComponentByClass<ComponentType>()) {
			return component;
		}
	}
	return nullptr;
}

template <class Predicate>
AActor* getFirstActor(UWorld* world, TSubclassOf<AActor> cls, const Predicate& predicate) {
	for (auto actor : TActorRange<AActor>(world, cls)) {
		if (predicate(*actor)) {
			return actor;
		}
	}
	return nullptr;
}

template <class T>
T* getFirstActor(UWorld* world) {
	auto it = TActorIterator<T>(world);
	return it ? *it : nullptr;
}

template <class T, class Predicate>
T* getFirstActor(UWorld* world, const Predicate& predicate) {
	for (auto actor : TActorRange<T>(world)) {
		if (predicate(*actor)) {
			return actor;
		}
	}
	return nullptr;
}

template <class T>
T* getFirstActorTracked(UWorld* world) {
	auto& it = InstanceTracker< T >::GetList(world);
	return it.Num() ? it[0] : nullptr;
}

template <class T, class Predicate>
T* getFirstActorTracked(UWorld* world, const Predicate& predicate) {
	for (auto actor : InstanceTracker< T >::GetList(world)) {
		if (predicate(*actor)) {
			return actor;
		}
	}
	return nullptr;
}

template <>
AGameBP* getFirstActor<AGameBP>(UWorld* world);



::game::Game* getGame(UWorld* world);

}
