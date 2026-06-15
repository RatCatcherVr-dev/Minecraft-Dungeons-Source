#include "Dungeons.h"
#include "ItemDropEffectComponent.h"
#include "game/item/ItemUtil.h"
#include "DungeonsUserManagement.h"
#include "Engine/LocalPlayer.h"
#include "DungeonsGameInstance.h"

UItemDropEffectComponent::UItemDropEffectComponent():
	bEnableEffect(true), 
	bAllowDropSourceEffectMultipliers(true),
	DropConeRadius(0.1f),
	DropVelocityMin(90.0f),
	DropVelocityMax(110.0f),
	BumpAmplificationVelocity(FVector(0, 0, 0))
{
}

void UItemDropEffectComponent::Apply(AActor* DropSource, bool doCustomImpulse) const
{
	if (bEnableEffect)
	{
		const auto Item = GetOwner();

		if(!doCustomImpulse)
		{
			Apply(Item, DropSource);
		}
		else
		{
			ImpulseCustom(Item);
		}
	}
}

void UItemDropEffectComponent::ImpulseCustom(AActor* Item) const
{
	if (auto ItemCollisionMesh = dynamic_cast<USphereComponent*>(Item->GetRootComponent()))
	{
		ULocalPlayer* owningLocalPlayer = dynamic_cast<ULocalPlayer*>(Item->GetNetOwningPlayer());

		if (owningLocalPlayer)
		{
			auto gameInstance = GetWorld()->GetGameInstance<UDungeonsGameInstance>();
			auto userManagement = gameInstance->GetUserManager();
			const int localPNum = gameInstance->GetNumLocalPlayers();
			const int localIndex = userManagement->GetLocalPlayerIndex(owningLocalPlayer);

			//D11.KS - 750 is amount, the rest is to scale the distance depending on the amount of local players, 100% for 4, 90% for 3, and 80% for 2.
			const float forward = 750 * 0.1 * (10 - (4 - localPNum));
			const float up = 1000;

			//D11.KS - localPNum should never be 0.
			const auto Rotation = (360 / localPNum) * localIndex;
		
			FVector impulseVector(forward, 0, up);
	
			impulseVector = impulseVector.RotateAngleAxis(Rotation, FVector::UpVector);

			ItemCollisionMesh->AddImpulse(impulseVector, NAME_None, true);
		}
	}
}


void UItemDropEffectComponent::Apply(AActor* Item, AActor* DropSource) const
{
	if (auto ItemCollisionMesh = dynamic_cast<USphereComponent*>(Item->GetRootComponent()))
	{
		const auto Velocity = GetVelocity(DropSource);
		ItemCollisionMesh->AddImpulse(Velocity, NAME_None, true);

		if (ShouldRotate(Item)) {
			const auto Rotation = FMath::FRandRange(0, 360);
			ItemCollisionMesh->SetRelativeRotation(FRotator(0, Rotation, 0));
		}
	}
}

bool UItemDropEffectComponent::ShouldRotate(AActor* Item) const
{
	const auto RotateProp = FindField<UBoolProperty>(Item->GetClass(), "Rotate");
	return RotateProp ? RotateProp->GetPropertyValue_InContainer(Item) : false;
}

FVector UItemDropEffectComponent::GetVelocity(AActor* DropSource) const
{
	auto Velocity = GetBaseVelocity(DropSource);
	if (!BumpAmplificationVelocity.IsZero())
	{
		const auto ConeAngle = GetConeAngle(BumpAmplificationVelocity);
		const auto Amplification = BumpAmplificationVelocity.Size() * (FMath::FRand() * 0.3f + 0.7f);
		Velocity += Amplification * FMath::VRandCone(FVector(0,0,1), PI/2-ConeAngle);
	}
	return Velocity;
}

float UItemDropEffectComponent::GetMultiplier(AActor* DropSource, const char* FieldName) const
{
	const auto MultiplierProp = FindField<UFloatProperty>(DropSource->GetClass(), FieldName);
	return MultiplierProp ? MultiplierProp->GetPropertyValue_InContainer(DropSource) : 1.0;
}

FVector UItemDropEffectComponent::GetBaseVelocity(AActor* DropSource) const
{
	if (IsValid(DropSource))
	{		
		auto VelocityMinMultiplier = 1;
		auto VelocityMaxMultiplier = 1;
		auto RadiusMultiplier = 1;
		if (bAllowDropSourceEffectMultipliers)
		{
			// Use multipliers from source actors to boost velocity and cone radius. See LootActor.
			VelocityMinMultiplier = GetMultiplier(DropSource, "DropVelocityMinMultiplier");
			VelocityMaxMultiplier = GetMultiplier(DropSource, "DropVelocityMaxMultiplier");
			RadiusMultiplier = GetMultiplier(DropSource, "DropConeRadiusMultiplier");
		}		
		return game::item::util::randItemDropVelocity(DropVelocityMin * VelocityMinMultiplier, DropVelocityMax * VelocityMaxMultiplier, DropConeRadius * RadiusMultiplier, DropSource->GetActorRotation().Yaw);	
	}
	return game::item::util::randItemDropVelocity(DropVelocityMin, DropVelocityMax, DropConeRadius);
}

float UItemDropEffectComponent::GetConeAngle(const FVector& Velocity)
{
	FVector p(Velocity.X, Velocity.Y, 0);
	return FMath::Acos((p | Velocity) / p.Size() / Velocity.Size());
}