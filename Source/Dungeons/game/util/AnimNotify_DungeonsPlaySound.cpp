// Fill	out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "AnimNotify_DungeonsPlaySound.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Animation/AnimSequenceBase.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif

DECLARE_CYCLE_STAT(TEXT("UAnimNotify_DungeonsPlaySound::Notify"), STAT_UAnimNotify_DungeonsPlaySound_Notify, STATGROUP_Anim);


/////////////////////////////////////////////////////
// UAnimNotify_DungeonsPlaySound

UAnimNotify_DungeonsPlaySound::UAnimNotify_DungeonsPlaySound()
	: Super()
{
	VolumeMultiplier = 1.f;
	PitchMultiplier = 1.f;

#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(196, 142, 255, 255);
#endif // WITH_EDITORONLY_DATA
}

void UAnimNotify_DungeonsPlaySound::Notify(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	SCOPE_CYCLE_COUNTER(STAT_UAnimNotify_DungeonsPlaySound_Notify);

	// Don't call super to avoid call back in to blueprints
	if (!Sound.IsNull())
	{
		if (!Sound.Get())
		{
			//Async load this sound the first time its requested rather than snyc load that causes big stalls	
			TWeakObjectPtr<UAnimNotify_DungeonsPlaySound> WeakThis(this); // using weak ptr in case 'this' has gone out of scope by the time this lambda is called
			TWeakObjectPtr<USkeletalMeshComponent> WeakMeshComp(MeshComp); // using weak ptr in case 'MeshComp' has gone  by the time this lambda is called
			
			UAssetManager::GetStreamableManager().RequestAsyncLoad(Sound.ToSoftObjectPath(),
				[&, WeakThis, WeakMeshComp]() {


				if (WeakThis.IsValid() && Sound.Get() && !Sound.Get()->IsLooping() && WeakMeshComp.IsValid())
				{
					if (bFollow)
					{
						UGameplayStatics::SpawnSoundAttached(Sound.Get(), WeakMeshComp.Get(), AttachName, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, false, VolumeMultiplier, PitchMultiplier);
					}
					else
					{
						UGameplayStatics::PlaySoundAtLocation(WeakMeshComp->GetWorld(), Sound.Get(), WeakMeshComp->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
					}
				}

			}
				, FStreamableManager::AsyncLoadHighPriority + 10,
				false,
				false,
				TEXT("UAnimNotify_DungeonsPlaySound::Notify"));

			return;
			
		}
				

		if (!Sound.Get())
		{
			return;
		}

		if (Sound.Get()->IsLooping())
		{
			UE_LOG(LogAudio, Warning, TEXT("PlaySound notify: Anim %s tried to spawn infinitely looping sound asset %s. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(Sound.Get()));
			return;
		}


		if (bFollow)
		{
			UGameplayStatics::SpawnSoundAttached(Sound.Get(), MeshComp, AttachName, FVector(ForceInit), EAttachLocation::KeepRelativeOffset, false, VolumeMultiplier, PitchMultiplier);
		}
		else
		{
			UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), Sound.Get(), MeshComp->GetComponentLocation(), VolumeMultiplier, PitchMultiplier);
		}
	}
}


void UAnimNotify_DungeonsPlaySound::PostLoad()
{
	Super::PostLoad();
}

void UAnimNotify_DungeonsPlaySound::BeginDestroy()
{
	Super::BeginDestroy();
}

FString UAnimNotify_DungeonsPlaySound::GetNotifyName_Implementation() const
{
	if (!Sound.IsNull())
	{
		Sound.ToSoftObjectPath().TryLoad();
		return (Sound.Get()) ? Sound.Get()->GetName() : Super::GetNotifyName_Implementation();
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

#if WITH_EDITOR
void UAnimNotify_DungeonsPlaySound::ValidateAssociatedAssets()
{
	static const FName NAME_AssetCheck("AssetCheck");
	if (!Sound.IsNull())
	{
		Sound.ToSoftObjectPath().TryLoad();
	}
	if (Sound.Get() && (Sound.Get()->IsLooping()))
	{
		UObject* ContainingAsset = GetContainingAsset();

		FMessageLog AssetCheckLog(NAME_AssetCheck);

		const FText MessageLooping = FText::Format(
			NSLOCTEXT("AnimNotify", "Sound_ShouldNotLoop", "Sound {0} used in anim notify for asset {1} is set to looping, but the slot is a one-shot (it won't be played to avoid leaking an instance per notify)."),
			FText::AsCultureInvariant(Sound.Get()->GetPathName()),
			FText::AsCultureInvariant(ContainingAsset->GetPathName()));
		AssetCheckLog.Warning()
			->AddToken(FUObjectToken::Create(ContainingAsset))
			->AddToken(FTextToken::Create(MessageLooping));

		if (GIsEditor)
		{
			AssetCheckLog.Notify(MessageLooping, EMessageSeverity::Warning, /*bForce=*/ true);
		}
	}
}
#endif


