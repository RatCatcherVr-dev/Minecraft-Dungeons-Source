#include "Dungeons.h"
#include "game/component/SpawnActorIntervalComponent.h"
#include "game/util/LocationQuery.h"

USpawnActorIntervalComponent::USpawnActorIntervalComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void USpawnActorIntervalComponent::Activate(bool reset) {
	if (GetOwnerRole() == ROLE_Authority) {
		if (reset || ShouldActivate() == true) {
			if (reset) {
				SpawnQueue.Reset();
				currentSpawnDistance.Reset();
				LastOwnerLocation = GetOwner()->GetActorLocation();
			}
			const auto& current = SpawnQueue.Current();
			if (current.bDistanceBased){
				if (!currentSpawnDistance) {
					Spawn(current, GetOwner()->GetActorLocation());
					const auto& next = SpawnQueue.Next();
					currentSpawnDistance = FMath::RandRange(current.MinSpawnDistance, current.MaxSpawnDistance);
					LastOwnerLocation = GetOwner()->GetActorLocation();
				}
				SetComponentTickEnabled(true);
			}
			else {
				auto delay = FMath::RandRange(current.MinSpawnDelay, current.MaxSpawnDelay);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &USpawnActorIntervalComponent::OnSpawnTimed, GetOwner()->GetActorLocation(), UGameplayStatics::GetRealTimeSeconds(GetWorld()), delay), delay, false);
			}
		}
	}

	bIsActive = true;
	OnComponentActivated.Broadcast(this, true);
}

void USpawnActorIntervalComponent::Deactivate() {
	Super::Deactivate();
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	TimerHandle.Invalidate();
}

void USpawnActorIntervalComponent::TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(deltaTime, TickType, ThisTickFunction);
	auto current = SpawnQueue.Current();
	bool shouldContinue = SpawnQueue.IsOnLast() ? bLoopQueue : true;
	if (shouldContinue && current.bDistanceBased) {
		const auto diff = GetOwner()->GetActorLocation() - LastOwnerLocation;
		auto dir = diff; dir.Normalize();
		if (diff.Size() > currentSpawnDistance.GetValue()) {
			int times = diff.Size() / currentSpawnDistance.GetValue();
			
			for (int i = 0; i < times; ++i) {
				auto spawnLocation = LastOwnerLocation + dir * currentSpawnDistance.GetValue();
				Spawn(current, spawnLocation);
				shouldContinue = SpawnQueue.IsOnLast() ? bLoopQueue : true;
				if (shouldContinue) {
					current = SpawnQueue.Next();
					if (current.bDistanceBased) {
						currentSpawnDistance = FMath::RandRange(current.MinSpawnDistance, current.MaxSpawnDistance);
						LastOwnerLocation = spawnLocation;
					}
				}
			}
			LastOwnerLocation = GetOwner()->GetActorLocation();
		}
	}
}

void USpawnActorIntervalComponent::SetLoopQueue(bool loop) {
	bLoopQueue = loop;
}

void USpawnActorIntervalComponent::SetSpawnQueue(FSpawnQueue queue) {
	SpawnQueue = std::move(queue);
}

void USpawnActorIntervalComponent::SetInstigator(ABaseCharacter* instigator) {
	Instigator = instigator;
}

void USpawnActorIntervalComponent::OnSpawnTimed(FVector callLocation, float callTimeStamp, float duration) {
	const auto& current = SpawnQueue.Current();
	const auto currentLocation = GetOwner()->GetActorLocation();
	Spawn(current, currentLocation);
	
	const auto rest = callTimeStamp - duration;
	if (static_cast<int>(rest / duration) > 1) {
		const auto time = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - callTimeStamp;
		const auto dist = currentLocation - callLocation;
		const auto velocity = dist.Size() / time;
		auto dir = dist; dir.Normalize();
		auto newSpawnLocation = callLocation;
		while (true) {
			const auto delay = FMath::RandRange(current.MinSpawnDelay, current.MaxSpawnDelay);
			const auto move = delay * velocity;
			newSpawnLocation = newSpawnLocation + dir * move;
			if ((newSpawnLocation - currentLocation).Size() < dist.Size()) {
				Spawn(current, newSpawnLocation);
			}
			else {
				break;
			}
		}
	}
	
	if (SpawnQueue.IsOnLast() && bLoopQueue) {
		const auto& next = SpawnQueue.Next();
		if (!next.bDistanceBased) {
			auto delay = FMath::RandRange(next.MinSpawnDelay, next.MaxSpawnDelay);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &USpawnActorIntervalComponent::OnSpawnTimed, GetOwner()->GetActorLocation(), UGameplayStatics::GetRealTimeSeconds(GetWorld()), delay), delay, false);
		}
		else {
			currentSpawnDistance = FMath::RandRange(current.MinSpawnDistance, current.MaxSpawnDistance);
		}
	}
}

void USpawnActorIntervalComponent::Spawn(const FSpawnElement& elem, const FVector& location) {
	auto offset = FMath::RandRange(elem.SpawnOffsetMin, elem.SpawnOffsetMax);
	auto angle = FMath::RandRange(0.f, 360.f);
	auto dir = FRotator(0, angle, 0).RotateVector(FVector(0, 1, 0));
	FActorSpawnParameters params;
	params.Owner = GetOwner();
	params.Instigator = Instigator;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	auto loc = location + (dir * offset);
	if (elem.bSpawnOnGround) {
		auto Z = locationquery::findGround(*GetWorld(), location, true);
		loc.Z = Z ? Z.GetValue() : loc.Z;
	}

	FTransform transform(FRotator(0, elem.bRandomizeRotation ? FMath::RandRange(0.f, 360.f) : 0.f, 0.f), loc, FVector(1, 1, 1));

	auto* actor = GetWorld()->SpawnActorDeferred<AActor>(elem.Classes[FMath::RandRange(0, elem.Classes.Num() - 1)], transform, GetOwner(), Instigator, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	OnActorSpawnedDefered.Broadcast(actor);
	actor->FinishSpawning(transform);
}

void FSpawnQueue::Reset() {
	CurrentIndex = 0;
}

FSpawnElement& FSpawnQueue::Current() {
	return Elements[CurrentIndex];
}

FSpawnElement& FSpawnQueue::Next() {
	if (CurrentIndex < Elements.Num() - 1) {
		CurrentIndex++;
	} else {
		CurrentIndex = 0;
	}
	return Elements[CurrentIndex];
}

bool FSpawnQueue::IsOnLast() const {
	return CurrentIndex == Elements.Num() - 1;
}
