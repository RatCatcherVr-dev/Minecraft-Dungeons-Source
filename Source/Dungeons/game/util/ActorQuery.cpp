#include "Dungeons.h"
#include "ActorQuery.h"
#include "DungeonsGameState.h"
#include "EngineUtils.h"
#include "LocationQuery.h"
#include "Tags.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/team/TeamQuery.h"
#include "util/FloatRange.h"
#include <functional>
#include <AIController.h>
#include <AbilitySystemComponent.h>
#include <Engine.h>
#include <UnrealMathUtility.h>
#include "Kismet/KismetMathLibrary.h"

// should be defined in a better place, e.g. game/util/Tags.cpp
// this doesn't build however, without UnityBuild. Move this back if a solution is found! /jakob
namespace tags {
	const FName inLove { "in-love" };
	const FName petTarget { "pet-target" };
	const FName collected { "collected" };
	const FName playerCorpse { "player-corpse" };
	const FName noDrop { "no-drop" };
	const FName noPickUp { "no-pick-up" };
	const FName isAttractive { "is-attractive" };
	const FName pristine { "pristine" };
	const FName randomMaterial { "random-material" };
	const FName flaslight { "flaslight" };
	const FName deactivateOnDisappear { "deactivate-on-disappear" };
	const FName cosmetic { "cosmetic" };
	const FName sequencerActor { "SequencerActor" }; // Added by Unreal to actors in level sequencers
}

float UActorQuery::GetActorDistance(const AActor* a, const AActor* b) {
	return actorquery::getActorDistance(a, b);
}

float UActorQuery::GetActorDistanceSquared(const AActor* a, const AActor* b) {
	return actorquery::getActorDistanceSquared(a, b);
}

bool UActorQuery::GetClosestSocket(const AActor* actor, const AActor* targetActor, FName& socketName, FVector& location) {
	return actorquery::getClosestSocket(actor->GetActorLocation(), targetActor, socketName, location);
}

FVector UActorQuery::GetActorHealthBarPosition(const APlayerController* viewingPlayer, const AActor* actor) {
	return actorquery::getActorHealthBarPosition(viewingPlayer, actor);
}

AGameBP* UActorQuery::GetGameBP(const UObject* WorldContextObject){
	if (!WorldContextObject)
		return nullptr;

	if(const auto world = WorldContextObject->GetWorld()){
		return actorquery::getFirstActor<AGameBP>(world);
	}
	return nullptr;
}

bool UActorQuery::IsLocationRangeOfPlayerCharacter(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck)
{
	const float DistanceCheckSq = DistanceCheck * DistanceCheck;
	const auto world = WorldContextObject->GetWorld();

	auto& baselist = InstanceTracker<APlayerCharacter>::GetList(world);
	for (auto PlayerCharacterActor : baselist)
	{
		if (actorquery::getActorDistanceSquared(location, PlayerCharacterActor) < DistanceCheckSq)
		{
			return true;
		}
	}

	return false;
}

template <class T>
bool GetPlayerCharactersInRangeList(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< T >& PlayerList, bool OrderByDistance /*= false*/)
{
	const float DistanceCheckSq = DistanceCheck * DistanceCheck;
	const auto world = WorldContextObject->GetWorld();
	PlayerList.Reset();

	auto& baselist = InstanceTracker<APlayerCharacter>::GetList(world);
	PlayerList.Reserve(baselist.Num());

	if (OrderByDistance)
	{
		TArray< float, TInlineAllocator<16> > Distances;

		for (auto PlayerCharacterActor : baselist)
		{
			const float fDistSq = actorquery::getActorDistanceSquared(location, PlayerCharacterActor);
			if (fDistSq < DistanceCheckSq)
			{
				int iInsertPos(0);
				const int iDistancesNum = Distances.Num();

				for (int i(0); i < iDistancesNum; ++i)
				{
					if (fDistSq < Distances[i])
					{
						iInsertPos = i;
						break;
					}
				}

				Distances.Insert(fDistSq, iInsertPos);
				PlayerList.Insert(PlayerCharacterActor, iInsertPos);
			}
		}
	}
	else
	{
		for (auto PlayerCharacterActor : baselist)
		{
			if (actorquery::getActorDistanceSquared(location, PlayerCharacterActor) < DistanceCheckSq)
			{
				PlayerList.Push(PlayerCharacterActor);
			}
		}
	}


	return PlayerList.Num() > 0;
}

bool UActorQuery::GetPlayerCharactersInRange(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< APlayerCharacter* >& PlayerList, bool OrderByDistance)
{
	return GetPlayerCharactersInRangeList(WorldContextObject,location, DistanceCheck, PlayerList, OrderByDistance);
}

bool UActorQuery::GetPlayerCharactersInRange(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< TWeakObjectPtr<APlayerCharacter> >& PlayerList, bool OrderByDistance /*= false*/)
{
	return GetPlayerCharactersInRangeList(WorldContextObject, location, DistanceCheck, PlayerList, OrderByDistance);
}

template <class T>
bool GetPlayerCharactersInRangeListPred(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< T >& PlayerList, bool OrderByDistance, std::function< bool(APlayerCharacter*) > PredFunc)
{
	const float DistanceCheckSq = DistanceCheck * DistanceCheck;
	const auto world = WorldContextObject->GetWorld();
	PlayerList.Reset();

	auto& baselist = InstanceTracker<APlayerCharacter>::GetList(world);
	PlayerList.Reserve(baselist.Num());

	if (OrderByDistance)
	{
		TArray< float, TInlineAllocator<16> > Distances;

		for (auto PlayerCharacterActor : baselist)
		{
			if (!PredFunc(PlayerCharacterActor))
			{
				continue;
			}

			const float fDistSq = actorquery::getActorDistanceSquared(location, PlayerCharacterActor);
			if (fDistSq < DistanceCheckSq)
			{
				int iInsertPos(0);
				const int iDistancesNum = Distances.Num();

				for (int i(0); i < iDistancesNum; ++i)
				{
					if (fDistSq < Distances[i])
					{
						iInsertPos = i;
						break;
					}
				}

				Distances.Insert(fDistSq, iInsertPos);
				PlayerList.Insert(PlayerCharacterActor, iInsertPos);
			}
		}
	}
	else
	{
		for (auto PlayerCharacterActor : baselist)
		{
			if (!PredFunc(PlayerCharacterActor))
			{
				continue;
			}

			if (actorquery::getActorDistanceSquared(location, PlayerCharacterActor) < DistanceCheckSq)
			{
				PlayerList.Push(PlayerCharacterActor);
			}
		}
	}


	return PlayerList.Num() > 0;
}
bool UActorQuery::GetPlayerCharactersInRangePred(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< APlayerCharacter* >& PlayerList, bool OrderByDistance, std::function< bool(APlayerCharacter*) > PredFunc)
{
	return GetPlayerCharactersInRangeListPred(WorldContextObject, location, DistanceCheck, PlayerList, OrderByDistance, PredFunc);
}

bool UActorQuery::GetPlayerCharactersInRangePred(const UObject* WorldContextObject, const FVector& location, const float DistanceCheck, TArray< TWeakObjectPtr<APlayerCharacter> >& PlayerList, bool OrderByDistance, std::function< bool(APlayerCharacter*) > PredFunc)
{
	return GetPlayerCharactersInRangeListPred(WorldContextObject, location, DistanceCheck, PlayerList, OrderByDistance, PredFunc);
}


bool UActorQuery::GetFriendlyMobCharactersInRange(const UObject* WorldContextObject, const ABaseCharacter* ActorReference, const float DistanceCheck, TArray< AMobCharacter* >& MobList, bool OrderByDistance /*= false*/)
{
	const float DistanceCheckSq = DistanceCheck * DistanceCheck;
	const auto world = WorldContextObject->GetWorld();
	MobList.Reset();

	auto& baselist = InstanceTracker<AMobCharacter>::GetList(world);
	MobList.Reserve(baselist.Num());

	if (OrderByDistance)
	{
		TArray< float, TInlineAllocator<16> > Distances;

		for (auto MobCharacterActor : baselist)
		{
			const float fDistSq = actorquery::getActorDistanceSquared(ActorReference->GetActorLocation(), MobCharacterActor);
			if (fDistSq < DistanceCheckSq)
			{
				int iInsertPos(0);
				const int iDistancesNum = Distances.Num();

				for (int i(0); i < iDistancesNum; ++i)
				{
					if (fDistSq < Distances[i])
					{
						iInsertPos = i;
						break;
					}
				}

				Distances.Insert(fDistSq, iInsertPos);
				MobList.Insert(MobCharacterActor, iInsertPos);
			}
		}
	}
	else
	{
		for (auto MobCharacterActor : baselist)
		{
			if (ActorReference->IsFriendlyTowards(MobCharacterActor) && actorquery::getActorDistanceSquared(ActorReference->GetActorLocation(), MobCharacterActor) < DistanceCheckSq)
			{
				MobList.Push(MobCharacterActor);
			}
		}
	}


	return MobList.Num() > 0;
}

bool UActorQuery::IsInRangeOfPlayerCharacter(const UObject* WorldContextObject, const AActor* thisActor, const float DistanceCheck)
{
	return IsLocationRangeOfPlayerCharacter(WorldContextObject, thisActor->GetActorLocation(), DistanceCheck);
}

namespace actorquery {

bool isActorTerrain(AActor* actor) {
	return actor->FindComponentByClass<UHealthComponent>() == nullptr;
}

bool isActorTargetableByPlayer(AActor* actor) {
	if (const auto mob = Cast<AMobCharacter>(actor)) {
		if (mob->IsTargetable()){
			const auto MobTeam = mob->GetCurrentTeam();
			return teamquery::can::damage(ETeamName::Heroes, MobTeam);
		}
	}

	return false;
}


float getActorDistance(const AActor* a, const AActor* b) {
	if (a == nullptr || b == nullptr)
		return 0;
	return FVector::Dist(a->GetActorLocation(), b->GetActorLocation());
}

float getActorCapsuleRadius(const AActor* actor) {
	if (UCapsuleComponent* pCapsule = actor->FindComponentByClass<UCapsuleComponent>()) {
		return pCapsule->GetScaledCapsuleRadius();
	}
	return 0.0f;
}

float getActorDistanceSquared(const AActor* a, const AActor* b) {
	if (a == nullptr || b == nullptr)
		return 0;
	return FVector::DistSquared(a->GetActorLocation(), b->GetActorLocation());
}

float getCapsuleDistance2D(const UCapsuleComponent * a, const UCapsuleComponent * b) {
	if (a == nullptr || b == nullptr) {
		return 0.f;
	}

	return FVector::Dist2D(a->GetComponentLocation(), b->GetComponentLocation()) - a->GetScaledCapsuleRadius() - b->GetScaledCapsuleRadius();
}

float getCapsuleDistance2D(const AActor* a, const AActor* b) {
	if (a == nullptr || b == nullptr) {
		return 0.f;
	}

	const auto capsuleA = a->FindComponentByClass<UCapsuleComponent>();
	const auto capsuleB = b->FindComponentByClass<UCapsuleComponent>();
	const auto radiusA = capsuleA != nullptr ? capsuleA->GetScaledCapsuleRadius() : 0.f;
	const auto radiusB = capsuleB != nullptr ? capsuleB->GetScaledCapsuleRadius() : 0.f;
	
	return FVector::Dist2D(a->GetActorLocation(), b->GetActorLocation()) - radiusA - radiusB;
}

/*
//D11.SC Im removing this for now, Iv tried refactoring the equation to get the correct distance but have been unable to avoid using the sqrt. maths fail. so its commented out for now in case someone tries to use it
float getCapsuleDistance2DSquared(const AActor* a, const AActor* b) {
	if (a == nullptr || b == nullptr) {
		return 0.f;
	}

	const auto capsuleA = a->FindComponentByClass<UCapsuleComponent>();
	const auto capsuleB = b->FindComponentByClass<UCapsuleComponent>();
	const auto radiusA = capsuleA != nullptr ? capsuleA->GetScaledCapsuleRadius() : 0.f;
	const auto radiusB = capsuleB != nullptr ? capsuleB->GetScaledCapsuleRadius() : 0.f;

	return FVector::DistSquared2D(a->GetActorLocation(), b->GetActorLocation()) - (radiusA*radiusA) - (radiusB*radiusB);
}*/


float getActorDistance(const FVector &sourcePoint, const AActor* actor) {
	if (actor == nullptr)
		return 0;
	return FVector::Dist(sourcePoint, actor->GetActorLocation());
}

float getActorDistanceSquared(const FVector &sourcePoint, const AActor* actor) {
	if (actor == nullptr)
		return 0;
	return FVector::DistSquared(sourcePoint, actor->GetActorLocation());
}

float getActorDistance2D(const AActor* a, const AActor* b) {
	if (a == nullptr || b == nullptr)
		return 0;
	return FVector::Dist2D(a->GetActorLocation(), b->GetActorLocation());
}

float getActorDistanceSquared2D(const AActor* a, const AActor* b) {
	if (a == nullptr || b == nullptr)
		return 0;
	return FVector::DistSquared2D(a->GetActorLocation(), b->GetActorLocation());

}

template<class T>
AActor* getFurthestInstanceTrackedBaseCharacterType(const AActor* source, float radius, float& distanceSquaredOut, std::function<bool(AActor*)> filter /* = [](AActor*) { return true; } */)
{
	if (AAIController::AreAIIgnoringPlayers())
	{
		return nullptr;
	}

	const auto sourceId = source->GetUniqueID();

	return getFurthestFromLocation<T>(source->GetActorLocation(), radius, distanceSquaredOut, source->GetWorld(), filter, sourceId);

}

template<class T>
AActor* getFurthestFromLocation(const FVector& location, float radius, float& distanceSquaredOut, UWorld* world, std::function<bool(AActor*)> filter /* = [](AActor*)  { return true; }*/, int sourceActorID /* = INDEX_NONE */, bool includeInvisiblePlayers /* = false */)
{
	const float radius_sq = radius * radius;

	auto maxDistSq = 0.0f;
	AActor* furthest = nullptr;

	//custom tracker is much faster than actor range		
	auto& baselist = InstanceTracker<T>::GetList(world);
	for (auto actor : baselist)
	{
		if (actor->GetUniqueID() == sourceActorID) {
			continue;
		}

		if (!filter(actor)) {
			continue;
		}

		auto d = getActorDistanceSquared(location, actor);
		if (d >= radius_sq || d <= maxDistSq) {
			continue;
		}

		auto healthComponent = actor->GetHealthComponent();
		if (healthComponent == nullptr || healthComponent->IsNotAlive()) {
			continue;
		}

		UAbilitySystemComponent* abilitySystem = actor->GetAbilitySystemComponent();
		if (abilitySystem && abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Ignore")))) {
			if (!includeInvisiblePlayers || !abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible")))) {
				continue;
			}
		}

		maxDistSq = d;
		furthest = actor;
	}


	distanceSquaredOut = maxDistSq;
	return furthest;
}

AActor* getFurthestPlayer(const AActor* source, float radius, float& distance_squared_out, std::function<bool(AActor*)> filter /* = [](AActor*) { return true; } */) {
	return getFurthestInstanceTrackedBaseCharacterType<APlayerCharacter>(source, radius, distance_squared_out, filter);
}

AActor* getFurthestMob(const AActor* source, float radius, float& distance_squared_out, std::function<bool(AActor*)> filter /* = [](AActor*) { return true; } */) {
	return getFurthestInstanceTrackedBaseCharacterType<AMobCharacter>(source, radius, distance_squared_out, filter);
}

template<class T>
AActor* getClosestInstanceTrackedBaseCharacterType(const AActor* source, float radius, float& distanceSquaredOut, std::function<bool(AActor*)> filter /* = [](AActor*) { return true; } */) {
	if (source) {
		if (AAIController::AreAIIgnoringPlayers()) {
			return nullptr;
		}
		return getClosestToLocation<T>(source->GetActorLocation(), radius, distanceSquaredOut, source->GetWorld(), filter, source->GetUniqueID());
	}
	return nullptr;
}

template<class T>
AActor* getClosestToLocation(const FVector& location, float radius, float& distanceSquaredOut, UWorld* world, std::function<bool(AActor*)> filter /* = [](AActor*)  { return true; }*/, int sourceActorID /* = INDEX_NONE */, bool includeInvisiblePlayers /* = false */)
{
	const float radius_sq = radius * radius;

	auto minDistSq = radius_sq;
	AActor* closest = nullptr;

	//custom tracker is much faster than actor range		
	auto& baselist = InstanceTracker<T>::GetList(world);
	for (auto actor : baselist)
	{
		if (actor->GetUniqueID() == sourceActorID) {
			continue;
		}

		if (!filter(actor)) {
			continue;
		}

		auto d = getActorDistanceSquared(location, actor);
		if (d >= minDistSq) {
			continue;
		}

		auto healthComponent = actor->GetHealthComponent();
		if (healthComponent == nullptr || healthComponent->IsNotAlive()) {
			continue;
		}

		static auto AI_Ignore = FGameplayTag::RequestGameplayTag(TEXT("AI.Ignore"));
		static auto StatusEffect_Invisible = FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Invisible"));

		UAbilitySystemComponent* abilitySystem = actor->GetAbilitySystemComponent();
		if (abilitySystem && abilitySystem->HasMatchingGameplayTag(AI_Ignore)) {
			if (!includeInvisiblePlayers || !abilitySystem->HasMatchingGameplayTag(StatusEffect_Invisible)) {
				continue;
			}
		}

		minDistSq = d;
		closest = actor;
	}


	distanceSquaredOut = minDistSq;
	return closest;

}



AActor* getClosestPlayer(const AActor* source, float radius, float& distance_squared_out, std::function<bool(AActor*)> filter /* = [](AActor*) { return true; } */) {
	return getClosestInstanceTrackedBaseCharacterType<APlayerCharacter>(source, radius, distance_squared_out, filter);
}

AActor* getClosestMob(const AActor* source, float radius, float& distance_squared_out, std::function<bool(AActor*)> filter /* = [](AActor*) { return true; } */) {
	if (source) {
		return getClosestInstanceTrackedBaseCharacterType<AMobCharacter>(source, radius, distance_squared_out, filter);
	}
	return nullptr;
}

AActor* getClosestPlayer(AActor* source) {
	return getActor<APlayerCharacter>(
		source->GetWorld(),
		is::aliveAndAiVisible,
		min::distance(source)
	);
}

AActor* getWeakestPlayer(AActor* source) {
	return getActor<APlayerCharacter>(
		source->GetWorld(),		
		is::aliveAndAiVisible,
		&min::health
	);
}

AActor* getClosestMob(AActor* source) {
	return getActor<AMobCharacter>(
		source->GetWorld(),
		is::aliveAndAiVisible,
		min::distance(source)
	);
}

AActor* getWeakestMob(AActor* source) {
	return getActor<AMobCharacter>(
		source->GetWorld(),
		is::aliveAndAiVisible,
		&min::health
	);
}

AActor* getWeakestCharacterAmong(UWorld* world, const TArray<ABaseCharacter*>& characters) {
	return getActor<ABaseCharacter>(
		world,
		is::aliveAndAiVisible,
		&min::health,
		characters
	);
}

bool is::alive(const AActor* actor) {
	if (!actor) {
		return false;
	}
	if (const auto healthComponent = actor->FindComponentByClass<UHealthComponent>()) {
		return healthComponent->IsAlive();
	}
	return true;
}

bool is::alive(const ABaseCharacter* character) {
	if (!character) {
		return false;
	}
	return character->IsAlive();
}


bool is::aiVisible(const AActor* actor) {
	if (!actor) {
		return false;
	}
	if (const auto abilityComponent = actor->FindComponentByClass<UAbilitySystemComponent>()) {
		return !abilityComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Ignore")));
	}
	return true;
}

bool is::aliveAndAiVisible(const AActor* actor) {
	return is::alive(actor) && is::aiVisible(actor);
}

bool is::healthMaxed(const AActor* actor) {
	if (const auto HC = actor->FindComponentByClass<UHealthComponent>()) {
		if (HC->IsHealthMaxed()) {
			return true;
		}
	}
	return false;
}

Pred<const AActor*> is::inRange(const AActor* a, float radius) {
	return [a, radiusSquared = radius * radius](const AActor* b) {
		return FVector::DistSquared(a->GetActorLocation(), b->GetActorLocation()) < radiusSquared;
	};
}

Pred<const AActor*> is::inRange(FVector src, float radius) {
	return[src, radiusSquared = radius * radius](const AActor* b) {
		return FVector::DistSquared(src, b->GetActorLocation()) < radiusSquared;
	};
}

Pred<const AActor*> is::entityType(EntityType type) {
	return [type](const AActor* actor) {
		const auto mob = Cast<AMobCharacter>(actor);
		return mob && mob->EntityType == type;
	};
}

const Pred<const AActor*>& is::player() {
	static const Pred<const AActor*> pred = [](const AActor* actor) {
		return actor && actor->IsA<APlayerCharacter>();
	};
	return pred;
}

const Pred<const AActor*>& is::mob() {
	static const Pred<const AActor*> pred = [](const AActor* actor) {
		return actor && actor->IsA<AMobCharacter>();
	};
	return pred;
}

const Pred<const AActor*>& is::mobShouldOverrideMusicTrack() {
	static const Pred<const AActor*> pred = [](const AActor* a) {
		const auto mob = Cast<AMobCharacter>(a);
		return mob
			&& mob->HasMusicOverride()
			&& !mob->IsUnderling()
			&& mob->IsRevealed()
			&& mob->IsAlive();
	};
	return pred;
}

const Pred<const AActor*>& is::mobWithOnScreenHealthbar() {
	static const Pred<const AActor*> pred = [](const AActor* a) {
		const auto mob = Cast<AMobCharacter>(a); 
		return mob
			&& (mob->IsEventMob() || mob->IsAncient() || mob->IsRaidCaptain())
			&& !mob->IsUnderling()
			&& mob->IsRevealed() 
			&& !mob->GetGlobalHealthBarComponent() 
			&& mob->IsAlive();
	};
	return pred;
}

const Pred<const AActor*> is::tagged(const FName tag) {
	return [tag](const AActor* a) {
		return a && a->ActorHasTag(tag);
	};
}

std::function<float(const AActor*)> min::distance(const AActor* a) {
	return [a](const AActor* b) {
		return FVector::DistSquared(a->GetActorLocation(), b->GetActorLocation());
	};
}

float min::health(const AActor* actor) {
	if (const auto healthComponent = actor->FindComponentByClass<UHealthComponent>()) {
		return healthComponent->GetCurrentHealth();
	}
	return std::numeric_limits<float>::infinity();
}

AActor* getFurthestActor(
	const AActor* source,
	float radius,
	const TSubclassOf<AActor>& actorType,
	float& distance,
	bool checkIfAlive /* =  true */,
	std::function<bool(AActor*)> filter /* = [](const AActor&) { return true; } */
) {
	if (AAIController::AreAIIgnoringPlayers())
	{
		return nullptr;
	}

	const float radius_sq = radius * radius;

	bool isFindingCharacter = actorType->IsChildOf<ABaseCharacter>();

	const auto sourceId = source->GetUniqueID();
	float maxDistSq = 0.0f;
	AActor* furthest = nullptr;

	auto actorPred = [&](AActor* actor) {

		if (actor->GetUniqueID() == sourceId) {
			return;
		}

		if (!filter(actor)) {
			return;
		}

		auto d = getActorDistanceSquared(source, actor);
		if (d >= radius_sq && d <= maxDistSq) {
			return;
		}

		if (isFindingCharacter)
		{
			ABaseCharacter* pChar = CastChecked<ABaseCharacter>(actor);

			if (checkIfAlive)
			{
				auto healthComponent = pChar->GetHealthComponent();
				if (healthComponent == nullptr || healthComponent->IsNotAlive()) {
					return;
				}
			}

			UAbilitySystemComponent* abilitySystem = pChar->GetAbilitySystemComponent();
			if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Ignore")))) {
				return;
			}
		}
		else if (checkIfAlive)
		{
			auto healthComponent = actor->FindComponentByClass<UHealthComponent>();
			if (healthComponent == nullptr || healthComponent->IsNotAlive()) {
				return;
			}

		};

		maxDistSq = d;
		furthest = actor;

	};


	//custom tracker is much faster than actor range
	if (isFindingCharacter)
	{
		auto& baselist = InstanceTracker<ABaseCharacter>::GetList(source->GetWorld());
		for (auto actor : baselist)
		{
			if (actor->GetClass()->IsChildOf(actorType))
			{
				actorPred(actor);
			}
		}
	}
	else
	{
		for (auto&& actor : TActorRange<AActor>(source->GetWorld(), actorType))
		{
			actorPred(actor);
		}
	}


	distance = Math::sqrt(maxDistSq);
	return furthest;
}

// radius, checkIfAlive and filter should just be filter
AActor* getClosestActor(
	const AActor* source,
	float radius,
	const TSubclassOf<AActor>& actorType,
	float& distance,
	bool checkIfAlive /* =  true */,
	std::function<bool(AActor*)> filter /* = [](const AActor&) { return true; } */
) {	
	if (AAIController::AreAIIgnoringPlayers())
	{
		return nullptr;
	}

	const float radius_sq = radius * radius;

	bool isFindingCharacter = actorType->IsChildOf<ABaseCharacter>();

	const auto sourceId = source->GetUniqueID();
	auto minDist = radius_sq;
	AActor* closest = nullptr;

	auto actorPred = [&](AActor* actor) {
	
		if (actor->GetUniqueID() == sourceId) {
			return;
		}

		if (!filter(actor)) {
			return;
		}

		auto d = getActorDistanceSquared(source, actor);
		if (d >= minDist) {
			return;
		}

		if (isFindingCharacter) 
		{
			ABaseCharacter* pChar = CastChecked<ABaseCharacter>(actor);

			if (checkIfAlive)
			{
				auto healthComponent = pChar->GetHealthComponent();
				if (healthComponent == nullptr || healthComponent->IsNotAlive()) {
					return;
				}
			}

			UAbilitySystemComponent* abilitySystem = pChar->GetAbilitySystemComponent();
			if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("AI.Ignore")))) {
				return;
			}
		}
		else if (checkIfAlive) 
		{
			auto healthComponent = actor->FindComponentByClass<UHealthComponent>();
			if (healthComponent == nullptr || healthComponent->IsNotAlive()) {
				return ;
		}

		};
		
		minDist = d;
		closest = actor;
	
	};


	//custom tracker is much faster than actor range
	if (isFindingCharacter)
	{
		auto& baselist = InstanceTracker<ABaseCharacter>::GetList(source->GetWorld());
		for (auto actor : baselist)
		{
			if (actor->GetClass()->IsChildOf(actorType))
			{
				actorPred(actor); 
			}
		}
	}
	else
	{
		for (auto&& actor : TActorRange<AActor>(source->GetWorld(), actorType)) 
		{
			actorPred(actor);
		}
	}


	distance = Math::sqrt(minDist);
	return closest;
}

DUNGEONS_API AActor* getClosestActorFromArray(const AActor* source, const TArray<AActor*> actorArray)
{
	AActor* closestActor = nullptr;
	float closestActorDistanceSq = std::numeric_limits<float>::max();

	for (auto actor : actorArray) {
		auto distance2DSq = FVector::DistSquared2D(source->GetActorLocation(), actor->GetActorLocation());

		if (closestActor == nullptr || distance2DSq < closestActorDistanceSq) {
			closestActor = actor;
			closestActorDistanceSq = distance2DSq;
		}
	}
	return closestActor;
}

void getActorsInCylinder(UWorld* world, FVector fromLocation, FVector cylinderDirection, const TSubclassOf<AActor>& actorType, float cylinderLength, float radius, float cylinderOffset, TArray<AActor*>& list) {
	cylinderDirection.Normalize();
	FloatRange cylinderRange(cylinderOffset, cylinderLength + cylinderOffset);
	const auto squaredRadius = radius * radius;

	for (auto&& actor : TActorRange<AActor>(world, actorType)) {
		auto health = actor->FindComponentByClass<UHealthComponent>();
		if (health != nullptr && health->IsNotAlive()) {
			continue;
		}

		const auto actorDelta = actor->GetActorLocation() - fromLocation;
		const auto scalarProjection = FVector::DotProduct(actorDelta, cylinderDirection);

		if (cylinderRange.inRange(scalarProjection) && ((cylinderDirection * scalarProjection) - actorDelta).SizeSquared() < squaredRadius) {
			list.Add(actor);
		}
	}
}

bool isActorInExtraMargin(const AActor& actor, FVector source, FVector direction, float maxDist, float maxAngleRadians, float maxZDiff) {
	TArray<TWeakObjectPtr<UCapsuleComponent>> targetCapsules;
	if (auto baseCharacter = Cast<ABaseCharacter>(&actor)) {
		targetCapsules = baseCharacter->GetCachedTargetableCapsules();
	}
	else if (auto character = Cast<ACharacter>(&actor)) {
		targetCapsules.Add(character->GetCapsuleComponent());
	}

	// sanitize 
	targetCapsules.RemoveAllSwap([](const auto &capsule) { return !capsule.IsValid(); });

	if (targetCapsules.Num() == 0) {
		return true;
	}

	// fail if all capsules fail the test
	for (const auto & capsule : targetCapsules) {
		const auto capsuleLocation = capsule->GetComponentLocation();

		// test z difference first (cheapest, but rejects very few)
		const auto capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();

		if (
			source.Z + maxZDiff < capsuleLocation.Z - capsuleHalfHeight ||
			source.Z - maxZDiff > capsuleLocation.Z + capsuleHalfHeight
			) {
			continue;
		}

		// test angle vs capsule center (rejects a lot)
		const auto deltaNormal = (capsuleLocation - source).GetSafeNormal2D();

		if (FVector2D::DotProduct(FVector2D{ direction }, FVector2D{ deltaNormal }) < FMath::Cos(maxAngleRadians)) {
			continue;
		}

		// test wedge radius vs capsule radius
		const auto capsuleRadius = capsule->GetScaledCapsuleRadius();
		const auto distanceSquared = FVector::DistSquared2D(source, capsuleLocation - deltaNormal * capsuleRadius);

		if (distanceSquared > FMath::Square(maxDist)) {
			continue;
		}

		// test wedge end-points vs capsule radius
		// ...

		// test wedge edges vs actor radius (angle vs actor center should not return early anymore)
		// ...

		return true;
	}

	return false;
}

bool isInArc(FVector source, FVector normalizedDirection, FVector location, float maxHalfAngleRadians) {
	if (maxHalfAngleRadians >= PI) {
		return true;
	}

	auto delta = location - source;
	
	// calc target direction as a 2D vector
	auto targetDirection = delta.GetSafeNormal2D();
	FVector2D targetDirection2D(targetDirection.X, targetDirection.Y);

	// get source direction as a 2D vector
	auto sourceDirection = normalizedDirection.GetSafeNormal2D();
	FVector2D sourceDirection2D(sourceDirection.X, sourceDirection.Y);

	auto dot = FVector2D::DotProduct(sourceDirection2D, targetDirection2D);	

	// bail
	if (dot < FMath::Cos(maxHalfAngleRadians)) {
		return false;
	}

	return true;
}

TAutoConsoleVariable<int32> CVarUseCorrectedMeleeWedgeMath(
	TEXT("Dungeons.Bugs.UseCorrectedMeleeWedgeMath"),
	1,
	TEXT("Enables fixed wedge math for melee combat which will make melee hit more in general.\n")
	TEXT("<=0: off.\n")
	TEXT(">0: on.\n"),
	ECVF_Cheat);

bool isActorInWedge(FVector source, FVector normalizedDirection, AActor* actor, float minDistUnits, float maxDistUnits, float maxHalfAngleRadians, float maxZDiff) {
	TArray<TWeakObjectPtr<UCapsuleComponent>> targetCapsules;
	if (auto baseCharacter = Cast<ABaseCharacter>(actor)) {
		targetCapsules = baseCharacter->GetCachedTargetableCapsules();
	}
	else if (auto character = Cast<ACharacter>(actor)) {
		targetCapsules.Add(character->GetCapsuleComponent());
	}

	// sanitize 
	targetCapsules.RemoveAllSwap([](const auto &capsule) { return !capsule.IsValid(); });

	if (CVarUseCorrectedMeleeWedgeMath.GetValueOnGameThread()) {
		//Corrected wedge math.
		for (const auto &capsule : targetCapsules) {
			const auto location = capsule->GetComponentLocation();
			const float capsuleRadius = capsule->GetScaledCapsuleRadius();

			// D11.BC : this should be a +, but fixing this might have some side effects
			const float fudgedMinDist = FMath::Max(0.0f, minDistUnits - capsuleRadius);
			const float fudgedMaxDist = FMath::Max(0.0f, maxDistUnits + capsuleRadius);

			const float minDistUnitsSq = fudgedMinDist * fudgedMinDist;
			const float maxDistUnitsSq = fudgedMaxDist * fudgedMaxDist;

			auto distance2DSq = FVector::DistSquared2D(source, location);
			auto zDiff = source.Z - location.Z;

			// bail
			if (distance2DSq < minDistUnitsSq || distance2DSq > maxDistUnitsSq || FMath::Abs(zDiff) > (maxZDiff + capsuleRadius)) {
				continue;
			}

			float extraAngleRadians = FMath::Atan2(capsuleRadius, FMath::Sqrt(distance2DSq));
			if (isInArc(source, normalizedDirection, location, maxHalfAngleRadians + extraAngleRadians)) {
				return true;
			}
		}
	}
	else {
		//Legacy wedge math.
		for (const auto &capsule : targetCapsules) {
			const auto location = capsule->GetComponentLocation();
			const auto normal = (location - source).GetSafeNormal();
			const FVector radiusCapsuleOffset = normal * capsule->GetScaledCapsuleRadius();
			const float minDistUnitsSq = minDistUnits * minDistUnits;
			const float maxDistUnitsSq = maxDistUnits * maxDistUnits;

			auto distance2DSq = FVector::DistSquared2D(source, actor->GetActorLocation() - radiusCapsuleOffset);
			auto zDiff = source.Z - (actor->GetActorLocation().Z - radiusCapsuleOffset.Z);

			// bail
			if (distance2DSq < minDistUnitsSq || distance2DSq > maxDistUnitsSq || FMath::Abs(zDiff) > maxZDiff) {
				continue;
			}

			if(isInArc(source, normalizedDirection, location, maxHalfAngleRadians)) { 
				return true;
			}
		}
	}

	return false;
}

bool getClosestBone(const FVector& closestToLocation, const USkeletalMeshComponent* mesh, FName& boneName, FVector& location) {
	auto socketNames = mesh->GetAllSocketNames();	
	float closest = -1.f;
	for (auto name : socketNames) {
		auto bone = mesh->GetSocketByName(name);
		if (bone != nullptr) {
			continue;
		}
		auto loc = mesh->GetBoneLocation(name);
		auto distance = FVector::DistSquared(closestToLocation, loc);
		if (closest < 0.f || distance < closest) {
			closest = distance;
			location = loc;
			boneName = name;
		}
	}

	if (closest < 0.f) {
		return false;
	}
	else {
		return true;
	}
}

bool getClosestBone(const FVector& closestToLocation, const AActor* targetActor, FName& boneName, FVector& location) {

	if (targetActor == nullptr) {
		return false;
	}

	auto targetCharacter = Cast<ACharacter>(targetActor);

	if (targetCharacter == nullptr) {
		return false;
	}

	auto mesh = targetCharacter->GetMesh();

	return getClosestBone(closestToLocation, mesh, boneName, location);
}

bool getClosestBoneOnPhysicsAsset(const FVector& closestToLocation, const AActor* targetActor, FName& boneName, FVector& location, bool approximate) {
	if (targetActor == nullptr) {
		return false;
	}

	auto targetCharacter = Cast<ACharacter>(targetActor);
	if (targetCharacter == nullptr) {
		return false;
	}

	if (auto mesh = targetCharacter->GetMesh()) {
		FClosestPointOnPhysicsAsset result;
		if (mesh->GetClosestPointOnPhysicsAsset(closestToLocation, result, approximate)) {
			boneName = result.BoneName;
			location = result.ClosestWorldPosition;
			return true;
		}
	}
	return false;
}

bool getClosestBoneFromList(const FVector& closestToLocation, const USkeletalMeshComponent* mesh, const TArray<FName>& boneList, FName& boneName, FVector& location){
	if (!mesh) {
		return false;
	}

	float closest = -1.f;
	for (auto name : boneList) {
		auto boneIndex = mesh->GetBoneIndex(name);
		if (boneIndex == INDEX_NONE) {
			continue;
		}
		auto loc = mesh->GetBoneMatrix(boneIndex).GetOrigin();
		auto distance = FVector::DistSquared(closestToLocation, loc);
		if (closest < 0.f || distance < closest) {
			closest = distance;
			location = loc;
			boneName = name;
		}
	}

	if (closest < 0.f) {
		return false;
	}
	else {
		return true;
	}
}


bool getClosestSocket(const FVector& closestToLocation, const AActor* targetActor, FName& socketName, FVector& location) {

	if (targetActor == nullptr) {
		return false;
	}

	auto targetCharacter = Cast<ACharacter>(targetActor);
	if (targetCharacter == nullptr) {
		return false;
	}
	auto mesh = targetCharacter->GetMesh();
	auto socketNames = mesh->GetAllSocketNames();
	float closest = -1.f;
	for (auto name : socketNames) {
		auto socket = mesh->GetSocketByName(name);
		if (socket == nullptr) {
			continue;
		}
		auto loc = socket->GetSocketLocation(mesh);
		auto distance = FVector::DistSquared(closestToLocation, loc);
		if (closest < 0.f || distance < closest) {
			closest = distance;
			location = loc;
			socketName = name;
		}
	}

	if (closest < 0.f) {
		return false;
	}
	else {
		return true;
	}
}

FVector getActorHealthBarPosition(const APlayerController* viewingPlayer, const AActor* actor) {
	const ACharacter* character = Cast<ACharacter>(actor);

	//For when we add an options to choose.
	bool HealthBarsBelow = false; 
	//GConfig->GetBool(TEXT("/Settings/Gameplay/Healthbars"), TEXT("DrawBelow"), HealthBarsBelow, GGameIni);
	
	const auto actorCapsule = character ? character->GetCapsuleComponent() : actor->FindComponentByClass<UCapsuleComponent>();
	const auto actorLocation = character ? character->GetMesh()->GetComponentLocation() + FVector(0,0, actorCapsule->GetScaledCapsuleHalfHeight()) : actor->GetActorLocation();

	if(HealthBarsBelow){
		const auto cameraRotation = viewingPlayer->PlayerCameraManager->GetCameraRotation();
		auto flatCamera = cameraRotation;
		flatCamera.Pitch = 0;
		flatCamera.Roll = 0;

		const float offsetZ = (actorCapsule ? -actorCapsule->GetScaledCapsuleHalfHeight() : 0.0f);
		const float offsetX = (actorCapsule ? actorCapsule->GetScaledCapsuleRadius() : 0.0f) + 25.0f;
		const auto barLocation = actorLocation - flatCamera.Vector() * offsetX;

		return barLocation;
	} else {
		//Using height as base offset - but adding 0.67 of radius because a wide character needs more spacing to health bar to be visually clear.
		const float offset = (actorCapsule ? actorCapsule->GetScaledCapsuleHalfHeight() + actorCapsule->GetScaledCapsuleRadius() * 0.67f : 0.0f) + 50.0f;		
		auto barLocation = actorLocation;
		barLocation.Z += offset;
		return barLocation;
	}
}

bool isAlive(const AActor* actor) {
	if (!actor) {
		return false;
	}
	if (auto health = actor->FindComponentByClass<UHealthComponent>()) {
		return health->IsAlive();
	}
	return true;
}

AActor* getClosestActorInCone2D(const AActor* source, const TSubclassOf<AActor>& actorType, float maxDistUnits, float maxAngleRadians, bool traceHit, Pred<const AActor*> pred) {

	AActor* closestActor = nullptr;
	float closestActorDistanceSq = std::numeric_limits<float>::max();

	const float maxDistUnitsSq = maxDistUnits * maxDistUnits;

	for (auto&& actor : TActorRange<AActor>(source->GetWorld(), actorType)) {
		if (!pred(actor)) {
			continue;
		}
		
		auto distance2DSq = FVector::DistSquared2D(source->GetActorLocation(), actor->GetActorLocation());

		// bail
		if (distance2DSq > maxDistUnitsSq) {
			continue;
		}

		// calc target direction as a 2D vector
		auto delta = actor->GetActorLocation() - source->GetActorLocation();
		auto targetDirection = delta.GetSafeNormal2D();
		FVector2D targetDirection2D(targetDirection.X, targetDirection.Y);

		// get source direction as a 2D vector
		auto sourceDirection = source->GetActorForwardVector().GetSafeNormal2D();
		FVector2D sourceDirection2D(sourceDirection.X, sourceDirection.Y);

		auto dot = FVector2D::DotProduct(sourceDirection2D, targetDirection2D);
		auto angle = FMath::Acos(dot);

		// bail
		if (angle > maxAngleRadians) {
			continue;
		}

		// bail if we're unable to trace to target
		if (traceHit) {

			auto channel = (ECollisionChannel)ECustomTraceChannels::IgnorePlayer;

			if (!source->GetClass()->IsChildOf(APlayerCharacter::StaticClass()))
				channel = (ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly;

			FHitResult hr;
			if (source->GetWorld()->LineTraceSingleByChannel(hr, source->GetActorLocation(), actor->GetActorLocation(), channel)) {
				if (hr.Actor.IsValid() && hr.GetActor() != actor) {
					continue;
				}
			}
		}

		// woot
		if (closestActor == nullptr || distance2DSq < closestActorDistanceSq) {
			closestActor = actor;
			closestActorDistanceSq = distance2DSq;
		}

	}
	return closestActor;
}
FActorsWithAngle getActorsInCone2DWithAngle(const AActor* source, FVector sourceOffset, FVector direction, const TSubclassOf<AActor>& actorType, float maxDistUnits, float maxAngleRadians, Pred<const AActor*> pred, bool bDebug) {

	TArray<AActor*> actorsInCone;
	FActorsWithAngle actorsWithAngle;

	FVector sourceLocation = source->GetActorLocation() + sourceOffset;
	AActor* closestActor = nullptr;
	float angle = 0.0f;
	float closestActorDistanceSq = std::numeric_limits<float>::max();

	const float maxDistUnitsSq = maxDistUnits * maxDistUnits;

	for (auto&& actor : TActorRange<AActor>(source->GetWorld(), actorType)) {
		if (!pred(actor)) {
			continue;
		}

		auto distance2DSq = FVector::DistSquared2D(sourceLocation, actor->GetActorLocation());

		// bail
		if (distance2DSq > maxDistUnitsSq) {
			continue;
		}

		// calc target direction as a 2D vector
		auto delta = (actor->GetActorLocation() - sourceLocation);
		auto targetDirection = delta.GetSafeNormal2D();
		FVector2D targetDirection2D(targetDirection.X, targetDirection.Y);

		// get source direction as a 2D vector
		auto sourceDirection = direction;
		FVector2D sourceDirection2D(sourceDirection.X, sourceDirection.Y);

		auto dot = FVector2D::DotProduct(targetDirection2D, sourceDirection2D);
		angle = FMath::Acos(dot);

		if(angle > maxAngleRadians) {
			continue;
		}

		if(closestActor == nullptr || distance2DSq < closestActorDistanceSq) {
			closestActor = actor;
			closestActorDistanceSq = distance2DSq;
			
			float side = FVector2D::CrossProduct(targetDirection2D, sourceDirection2D);

			// Right Side
			if (direction.X >= 0)
			{
				angle = side > 0 ? UKismetMathLibrary::MapRangeClamped(angle, 0, maxAngleRadians, 0.5f, 1.f) : UKismetMathLibrary::MapRangeClamped(angle, maxAngleRadians, 0, 0.0f, 0.5f);
			}
			// Left Side
			else
			{
				angle = side > 0 ? UKismetMathLibrary::MapRangeClamped(angle, 0, maxAngleRadians, 0.5f, 0.0f) : UKismetMathLibrary::MapRangeClamped(angle, maxAngleRadians, 0, 1.f, 0.5f);
			}
			actorsWithAngle.actorAngle = angle;
		}

		actorsWithAngle.actors.Add(actor);
	}	

	if (bDebug)
	{
		DrawDebugCone(source->GetWorld(), sourceLocation, direction, maxDistUnits, maxAngleRadians, maxAngleRadians, 12, FColor::Yellow, false, 0.5f);
	}
	
	return actorsWithAngle;
}

TArray<AActor*> getActorsInCone2D(const AActor* source, const TSubclassOf<AActor>& actorType, float maxDistUnits, float maxAngleRadians, Pred<const AActor*> pred) {

	TArray<AActor*> actorsInCone;
	const float maxDistUnitsSq = maxDistUnits * maxDistUnits;

	for (auto&& actor : TActorRange<AActor>(source->GetWorld(), actorType)) {
		if (!pred(actor)) {
			continue;
		}

		auto distance2DSq = FVector::DistSquared2D(source->GetActorLocation(), actor->GetActorLocation());
		if (distance2DSq > maxDistUnitsSq) {
			continue;
		}

		// calc target direction as a 2D vector
		auto delta = actor->GetActorLocation() - source->GetActorLocation();
		auto targetDirection = delta.GetSafeNormal2D();
		FVector2D targetDirection2D(targetDirection.X, targetDirection.Y);

		// get source direction as a 2D vector
		auto sourceDirection = source->GetActorForwardVector().GetSafeNormal2D();
		FVector2D sourceDirection2D(sourceDirection.X, sourceDirection.Y);

		auto dot = FVector2D::DotProduct(sourceDirection2D, targetDirection2D);
		auto angle = FMath::Acos(dot);

		// bail
		if (angle > maxAngleRadians) {
			continue;
		}

		actorsInCone.Add(actor);
	}
	return actorsInCone;
}

template<>
AGameBP* getFirstActor<AGameBP>(UWorld* world)
{
	auto& it = InstanceTracker< AGameBP >::GetList(world);
	return it.Num() ? it[0] : nullptr;
}

game::Game* getGame(UWorld* world) {
	auto gbp = getFirstActor<AGameBP>(world);
	return gbp ? gbp->GetGame() : nullptr;
}

const affector::Affectors& getAffectors(UWorld* world) {
	auto* game = getGame(world);
	if (game == nullptr) {
		static const affector::Affectors affectors;
		return affectors;
	}
	return game->affectors();
}

}
