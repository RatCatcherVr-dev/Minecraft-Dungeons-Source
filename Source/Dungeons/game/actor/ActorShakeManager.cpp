
#include "ActorShakeManager.h"
#include "EngineGlobals.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "game/ActorShake.h"

//////////////////////////////////////////////////////////////////////////
// UActorShakeManager

DECLARE_CYCLE_STAT(TEXT("AddCameraShake"), STAT_AddCameraShake, STATGROUP_Game);

AActorShakeManager::AActorShakeManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

UActorShake* AActorShakeManager::AddActorShake(AActor* ActorToShake, TSubclassOf<class UActorShake> ShakeClass, float Scale, FRotator UserPlaySpaceRot)
{
	SCOPE_CYCLE_COUNTER(STAT_AddCameraShake);

	if (ShakeClass != nullptr)
	{
		UActorShake const* const ShakeCDO = GetDefault<UActorShake>(ShakeClass);
		if (ShakeCDO && ShakeCDO->bSingleInstance)
		{
			// look for existing instance of same class
			for (UActorShake* ShakeInst : ActiveShakes)
			{
				if (ShakeInst && ShakeClass == ShakeInst->GetClass() && ShakeInst->GetActorToShake() == ActorToShake)
				{
					return ShakeInst;
				}
			}
		}

		// Try to find a shake in the expired pool
		UActorShake* NewInst = ReclaimShakeFromExpiredPool(ShakeClass);

		// No old shakes, create a new one
		if (NewInst == nullptr)
		{
			NewInst = NewObject<UActorShake>(this, ShakeClass);
		}

		if (NewInst)
		{
			// Initialize new shake and add it to the list of active shakes
			NewInst->PlayShake(ActorToShake, Scale, UserPlaySpaceRot);

			// look for nulls in the array to replace first -- keeps the array compact
			bool bReplacedNull = false;
			for (int32 Idx = 0; Idx < ActiveShakes.Num(); ++Idx)
			{
				if (ActiveShakes[Idx] == nullptr)
				{
					ActiveShakes[Idx] = NewInst;
					bReplacedNull = true;
				}
			}

			// no holes, extend the array
			if (bReplacedNull == false)
			{
				ActiveShakes.Emplace(NewInst);
			}
		}

		return NewInst;
	}

	return nullptr;
}

void AActorShakeManager::SaveShakeInExpiredPool(UActorShake* ShakeInst)
{
	FPooledActorShakes& PooledCameraShakes = ExpiredPooledShakesMap.FindOrAdd(ShakeInst->GetClass());
	if (PooledCameraShakes.PooledShakes.Num() < 5)
	{
		PooledCameraShakes.PooledShakes.Emplace(ShakeInst);
	}
}

UActorShake* AActorShakeManager::ReclaimShakeFromExpiredPool(TSubclassOf<UActorShake> CameraShakeClass)
{
	if (FPooledActorShakes* PooledCameraShakes = ExpiredPooledShakesMap.Find(CameraShakeClass))
	{
		if (PooledCameraShakes->PooledShakes.Num() > 0)
		{
			UActorShake* OldShake = PooledCameraShakes->PooledShakes.Pop();
			// Calling new object with the exact same name will re-initialize the uobject in place
			OldShake = NewObject<UActorShake>(this, CameraShakeClass, OldShake->GetFName());
			return OldShake;
		}
	}
	return nullptr;
}

void AActorShakeManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TArray<UActorShake*> ShakesToRemove;

	for (UActorShake* actorShake : ActiveShakes) {
		if (!actorShake->IsFinished()) {
			actorShake->UpdateAndApplyActorShake(DeltaSeconds, 1.0f);
		}
		else {
			ShakesToRemove.Add(actorShake);
		}
	}

	for (UActorShake* actorShake : ShakesToRemove)
	{
		RemoveActorShake(actorShake, true);
	}
}

void AActorShakeManager::RemoveActorShake(UActorShake* ShakeInst, bool bImmediately)
{
	for (int32 i = 0; i < ActiveShakes.Num(); ++i)
	{
		if (ActiveShakes[i] == ShakeInst)
		{
			ShakeInst->StopShake(bImmediately);

			if (bImmediately)
			{
				ActiveShakes.RemoveAt(i, 1);
				SaveShakeInExpiredPool(ShakeInst);
			}
			break;
		}
	}
}

void AActorShakeManager::RemoveAllActorShakesOfClass(TSubclassOf<class UActorShake> ShakeClass, bool bImmediately /*= true*/)
{
	for (int32 i = ActiveShakes.Num()-1; i >= 0; --i)
	{
		if ( ActiveShakes[i] && (ActiveShakes[i]->GetClass()->IsChildOf(ShakeClass)) )
		{
			ActiveShakes[i]->StopShake(bImmediately);
			if (bImmediately)
			{
				SaveShakeInExpiredPool(ActiveShakes[i]);
				ActiveShakes.RemoveAt(i, 1);
			}
		}
	}
}

void AActorShakeManager::RemoveAllActorShakes(bool bImmediately /*= true*/)
{
	// clean up any active camera shake anims
	for (UActorShake* Inst : ActiveShakes)
	{
		Inst->StopShake(bImmediately);
	}

	if (bImmediately)
	{
		for (UActorShake* Inst : ActiveShakes)
		{
			SaveShakeInExpiredPool(Inst);
		}

		// clear ActiveShakes array
		ActiveShakes.Empty();
	}
}
