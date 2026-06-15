#include "Dungeons.h"
#include "MeleeTicketProvider.h"

AMeleeTicketProvider::AMeleeTicketProvider(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AMeleeTicketProvider::CanAttack(AActor* requester, AActor* target, float duration, int maxCount/* = 3*/) {
	const auto id = target->GetUniqueID();
		
	FScopeLock lock(&mutex);

	auto& tickets = map.FindOrAdd(id);

	const auto now = GetWorld()->GetTimeSeconds();
	tickets.RemoveAllSwap([&](const auto& entry) {
		return entry.ExpiryTime < now || !entry.Requester.IsValid();
	});

	const bool alreadyRequested = tickets.ContainsByPredicate([&](const auto& entry) {
		return entry.Requester == requester;
	});

	if (alreadyRequested) {
		return true;
	}

	if (tickets.Num() >= maxCount) {
		return false;
	}
	
	tickets.Add({ requester, now + duration });
	
	return true;
}

void AMeleeTicketProvider::RequestTicket(AActor* requester, AActor* target, float duration) {
	const auto id = target->GetUniqueID();

	FScopeLock lock(&mutex);

	auto& tickets = map.FindOrAdd(id);

	const bool alreadyRequested = tickets.ContainsByPredicate([&](const auto& entry) {
		return entry.Requester == requester;
	});

	const auto now = GetWorld()->GetTimeSeconds();
	if (!alreadyRequested) {
		tickets.Add({ requester, now + duration });
	}	
}

void AMeleeTicketProvider::PostRegisterAllComponents()
{
	InstanceTracker< AMeleeTicketProvider >::AddInstance(GetWorld(), this);
	Super::PostRegisterAllComponents();
}

void AMeleeTicketProvider::PostUnregisterAllComponents()
{
	InstanceTracker< AMeleeTicketProvider >::RemoveInstance(GetWorld(), this);
	Super::PostUnregisterAllComponents();
}
