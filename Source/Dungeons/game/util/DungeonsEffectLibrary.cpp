#include "Dungeons.h"
#include "DungeonsEffectLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "ReactsToHitFlash.h"
#include "GameFramework/Character.h"
#include "game/actor/character/basecharacter.h"
#include "ActorQuery.h"

namespace internal {

UMaterialInstanceDynamic* GetMaterialInstanceDynamicFromMesh(class UPrimitiveComponent* mesh, int materialIndex) {
	if (UMaterialInterface* mat = mesh->GetMaterial(materialIndex)) {
		auto dynMatInstance = Cast<UMaterialInstanceDynamic>(mat);
		if (!dynMatInstance) {
			dynMatInstance = UMaterialInstanceDynamic::Create(mat, mesh);
			mesh->SetMaterial(materialIndex, dynMatInstance);
		}
		return dynMatInstance;
	}

	return nullptr;
}

}


UMaterialInstanceDynamic* UDungeonsEffectLibrary::GetMaterialInstanceDynamicFromMesh(UPrimitiveComponent* mesh, int materialIndex) {
	return internal::GetMaterialInstanceDynamicFromMesh(mesh, materialIndex);
}

TArray<UMaterialInstanceDynamic*> UDungeonsEffectLibrary::GetAllMaterialInstanceDynamicFromMesh(UPrimitiveComponent* mesh) {
	TArray<UMaterialInstanceDynamic*> result;
	for (int i = 0; i < mesh->GetNumMaterials(); ++i) {
		if (auto* material = internal::GetMaterialInstanceDynamicFromMesh(mesh, i)) {
			result.Add(material);
		}
	}
	return result;
}

void UDungeonsEffectLibrary::HitFlashOnMesh(UPrimitiveComponent* mesh, FLinearColor color, float duration, float startFraction /*= 1.f*/, float worldTime /*= -1.f*/) {
	if (auto dynMatInstance = internal::GetMaterialInstanceDynamicFromMesh(mesh, 0)) {
		const auto invertedStartFraction = 1.f - startFraction;
		const auto durationOffset = duration * invertedStartFraction;
		const auto actualDuration = duration + durationOffset;
		if(worldTime < 0.0f)
			worldTime = mesh->GetOwner()->GetWorld()->GetTimeSeconds();
		dynMatInstance->SetScalarParameterValue("Flash Seconds", actualDuration);
		dynMatInstance->SetVectorParameterValue("Flash Color", color);
		dynMatInstance->SetScalarParameterValue("Flash Timestamp", worldTime - (actualDuration * invertedStartFraction)); //material time node set to ignore pause, therefor needs to be realtime
	}
}

void UDungeonsEffectLibrary::HitFlashOnCharacter(ACharacter* character, FLinearColor color, float duration, float startFraction /*= 1.f*/, float worldTime /*= -1.f*/) {
	if (worldTime < 0.0f)
		worldTime = character->GetWorld()->GetTimeSeconds();

	if (auto player = Cast<IReactsToHitFlash>(character)) {
		player->OnHitFlash(color, duration);		
	}
	TArray<UPrimitiveComponent*> components;
	character->GetComponents(components, true);
	for (auto component : components) {
		HitFlashOnMesh(component, color, duration, startFraction, worldTime);
	}
}

void UDungeonsEffectLibrary::HitFlashOnActor(AActor* actor, FLinearColor color, float duration, float startFraction /*= 1.f*/, float worldTime /*= -1.f*/) {
	if (actor) {
		if (worldTime < 0.0f)
			worldTime = actor->GetWorld()->GetTimeSeconds();

		if (auto baseCharacter = Cast<ABaseCharacter>(actor)) {
			baseCharacter->HitFlashOnCharacter(color, duration, startFraction);
		}
		else if (auto character = Cast<ACharacter>(actor)) {
			HitFlashOnCharacter(character, color, duration, startFraction, worldTime);
		} else if (auto mesh = actor->FindComponentByClass<USkeletalMeshComponent>()) {
			UDungeonsEffectLibrary::HitFlashOnMesh(mesh, color, duration, startFraction, worldTime);
		}
	}
}

void UDungeonsEffectLibrary::PushBackOnActor(const FPushback PushBack, const AActor* byWhom, const AActor* affecting) {
	if (byWhom->IsValidLowLevel() && affecting->IsValidLowLevel()) {
		pushback::pushback(PushBack, *byWhom, *affecting);
	}
}

void UDungeonsEffectLibrary::PushBackOnActorInDirection(const FPushback PushBack, const FVector& pushDirection, const AActor* affecting) {
	if (affecting->IsValidLowLevel()) {
		pushback::pushback(PushBack, pushDirection, *affecting);
	}
}

void UDungeonsEffectLibrary::JumpPadLaunchActorInDirection(const FPushback PushBack, const FVector& pushDirection, const AActor* affecting)
{
	if (affecting->IsValidLowLevel()) {
		pushback::pushback(PushBack, pushDirection, *affecting, 1.0f, false, true);
	}
}

void UDungeonsEffectLibrary::AlignEffectAtActorFeet(AActor* actor, UParticleSystemComponent* effect) {
	bool valid = actor && effect;
	ensure(valid);
	if (valid) {
		if (const auto* capsule = Cast<UCapsuleComponent>(actor->GetRootComponent())) {
			const auto capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();

			FVector newLocation = FVector(0, 0, capsuleHalfHeight * -1);

			effect->SetRelativeLocation(newLocation);
		}
	}
}

FVector UDungeonsEffectLibrary::FindFeetOffsetForActor(AActor* actor, const float bias /* = 1.f */) {
	ensure(actor);
	if (actor) {
		if (const auto* capsule = Cast<UCapsuleComponent>(actor->GetRootComponent())) {
			const auto capsuleHalfHeight = capsule->GetScaledCapsuleHalfHeight();

			FVector newLocation = FVector(0, 0, capsuleHalfHeight * -1 + bias);

			return newLocation;
		}
	}

	return FVector::ZeroVector;
}


void UDungeonsEffectLibrary::AlignEffectToCurrentGround(AActor* actor, UParticleSystemComponent* effect, const float bias) {
	//Best guess on top of actor.
	const float radius = actor->GetSimpleCollisionRadius();
	const FVector location = actor->GetActorLocation();
	const FVector origin = location + FVector(0, 0, radius);


	effect->SetRelativeLocation(FindFeetOffsetForActor(actor, bias));
}

FVector UDungeonsEffectLibrary::FindCurrentGroundOffset(AActor* actor, const float bias /* = 1.f */) {
	//Best guess on top of actor.
	const float radius = actor->GetSimpleCollisionRadius();
	const FVector location = actor->GetActorLocation();
	const FVector origin = location + FVector(0, 0, radius);


	FHitResult result;

	if (actor->GetWorld()->LineTraceSingleByChannel(result, origin, origin - FVector(0, 0, 5000.f), (ECollisionChannel)ECustomTraceChannels::TerrainAndPlayerOnly)) {
		FVector offset = FVector(0, 0, -(result.Distance - radius) + bias);
		return offset;
	}

	return FVector::ZeroVector;
}

void UDungeonsEffectLibrary::DissolveActor(AActor* actor, const float duration)
{
	TArray<UMeshComponent*> componentsToDissolve;
	actor->GetComponents(componentsToDissolve, true);

	for (auto component : componentsToDissolve) {
		int index = 0;
		for (auto&& material : component->GetMaterials()) {
			auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
			if (materialDynamic == nullptr) {
				materialDynamic = UMaterialInstanceDynamic::Create(material, actor);
				component->SetMaterial(index, materialDynamic);
			}
			materialDynamic->SetScalarParameterValue("DissolveDuration", duration);
			materialDynamic->SetScalarParameterValue("DissolveStart", actor->GetWorld()->GetTimeSeconds());
			index++;
		}
	}
}

void UDungeonsEffectLibrary::AddTagToSceneComponents(AActor* actor, const FName& tag, bool includeChildActors) {
	TArray<USceneComponent*> components;
	actor->GetComponents(components, includeChildActors);

	for(auto* component : components) {
		component->ComponentTags.Add(tag);
	}
}
