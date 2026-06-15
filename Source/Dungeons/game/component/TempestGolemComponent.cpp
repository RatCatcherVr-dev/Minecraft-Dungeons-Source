#include "Dungeons.h"
#include "TempestGolemComponent.h"
#include "DungeonsGameMode.h"
#include "game/util/DungeonsEffectLibrary.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/AoeAttackComponent.h"
#include "game/ArmorProperties/DamageAbsorption.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/util/ComponentUtils.h"
#include "HealthComponent.h"
#include "AoeMarkerAttackComponent.h"


UTempestGolemComponent::UTempestGolemComponent()
{
	bReplicates = true;
}

void UTempestGolemComponent::BeginPlay()
{
	Super::BeginPlay();

	abilitySystemComp = GetOwner()->FindComponentByClass<UAbilitySystemComponent>();

	TempestGolemHealthComponent = GetOwner()->FindComponentByClass<UHealthComponent>();
	if (TempestGolemHealthComponent)
	{
		TempestGolemHealthComponent->OnDamageReceived.AddUObject(this, &UTempestGolemComponent::HandleDamageReceived);
		TempestGolemHealthComponent->OnDeath.AddUObject(this, &UTempestGolemComponent::OnDeath);
	}

	LeftArmAttackComponent = componentutils::GetComponentByTag<UAoeAttackComponent>(GetOwner(), "LeftArmAttack");
	RightArmAttackComponent = componentutils::GetComponentByTag<UAoeAttackComponent>(GetOwner(), "RightArmAttack");
	ShieldAttackComponent = componentutils::GetComponentByTag<UAoeAttackComponent>(GetOwner(), "ShieldAttack");
}

//RightArm
bool UTempestGolemComponent::CanArmAttack(TempestArmType armType)
{
	switch (armType)
	{
	case TempestArmType::RIGHT:
		return rightArmStatus == TempestArmStatus::ACTIVE;
	case TempestArmType::LEFT:
		return leftArmStatus == TempestArmStatus::ACTIVE;
	case TempestArmType::SHIELD:
		return shieldArmsStatus == TempestArmStatus::ACTIVE;
	default:
		return false;
	}
	
}

bool UTempestGolemComponent::IsArmTracking(TempestArmType armType)
{
	switch (armType)
	{
	case TempestArmType::RIGHT:
		return rightArmStatus == TempestArmStatus::TRACKING;
	case TempestArmType::LEFT:
		return leftArmStatus == TempestArmStatus::TRACKING;
	case TempestArmType::SHIELD:
		return shieldArmsStatus == TempestArmStatus::TRACKING;
	default:
		return false;
	}
}

void UTempestGolemComponent::ArmAttack(TempestArmType armType)
{
	if ((armType == TempestArmType::SHIELD && GetArmStatus(armType) == TempestArmStatus::TRACKING) || (GetArmStatus(armType) == TempestArmStatus::LOCKED))
	{
		SetArmStatus(armType, TempestArmStatus::ATTACKING);
	}
}
TempestArmStatus UTempestGolemComponent::GetArmStatus(TempestArmType armType)
{
	switch (armType)
	{
	case TempestArmType::RIGHT:
		return rightArmStatus;
	case TempestArmType::LEFT:
		return leftArmStatus;
	case TempestArmType::SHIELD:
		return shieldArmsStatus;
	}
	return TempestArmStatus::INVALID;
}
void UTempestGolemComponent::SetArmStatus(TempestArmType armType, TempestArmStatus newStatus)
{
	switch (armType)
	{
	case TempestArmType::RIGHT:
		rightArmStatus = newStatus;
		OnRep_TempestRightArmStatus();
		break;
	case TempestArmType::LEFT:
		leftArmStatus = newStatus;
		OnRep_TempestLeftArmStatus();
		break;
	case TempestArmType::SHIELD:
		shieldArmsStatus = newStatus;
		OnRep_TempestShieldArmsStatus();
		break;
	}
}

//Tempest
void UTempestGolemComponent::CheckTempestStun()
{
	if (rightArmStatus == TempestArmStatus::DISABLED &&
		leftArmStatus == TempestArmStatus::DISABLED)
	{
		SetArmStatus(TempestArmType::SHIELD, TempestArmStatus::DISABLED);
		SetTempestStatus(TempestStatus::STUNNED);

		UAoeMarkerAttackComponent* MarkerAttack = componentutils::GetComponentByTag<UAoeMarkerAttackComponent>(GetOwner(), "RangeAttack");
		MarkerAttack->StopAttack();
	}
}
TempestStatus UTempestGolemComponent::GetTempestStatus()
{
	return tempestStatus;
}
bool UTempestGolemComponent::IsTempestActive()
{
	return tempestStatus == TempestStatus::ACTIVE;
}
void UTempestGolemComponent::SetTempestStatus(TempestStatus newStatus)
{
	if (GetOwner() && GetOwner()->HasAuthority()) {
		tempestStatus = newStatus;
		OnRep_TempestStatus();

		if (newStatus == TempestStatus::ACTIVE)
		{
			SetArmStatus(TempestArmType::SHIELD, TempestArmStatus::ACTIVE);
		}
		else if (newStatus == TempestStatus::INTRO)
		{
			if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetOwner()))
			{
				BaseCharacter->GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
			}
		}
	}
}

void UTempestGolemComponent::OnRep_TempestStatus()
{
	if (ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetOwner()))
	{
		switch (tempestStatus)
		{
		case TempestStatus::INVALID:
		case TempestStatus::INTRO:
			BaseCharacter->SetTargetable(false);
			break;
		case TempestStatus::ACTIVE:
		case TempestStatus::STUNNED:
			BaseCharacter->SetTargetable(true);
			break;

		}
	}

	OnTempestStatusChanged.Broadcast(tempestStatus);
}

void UTempestGolemComponent::OnRep_TempestRightArmStatus()
{
	OnRightArmStatusChanged.Broadcast(rightArmStatus);
}

void UTempestGolemComponent::OnRep_TempestLeftArmStatus()
{
	OnLeftArmStatusChanged.Broadcast(leftArmStatus);
}

void UTempestGolemComponent::OnRep_TempestShieldArmsStatus()
{
	OnShieldArmsStatusChanged.Broadcast(shieldArmsStatus);
}

void UTempestGolemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTempestGolemComponent, tempestStatus);
	DOREPLIFETIME(UTempestGolemComponent, rightArmStatus);
	DOREPLIFETIME(UTempestGolemComponent, leftArmStatus);
	DOREPLIFETIME(UTempestGolemComponent, shieldArmsStatus);
}

void UTempestGolemComponent::StartArmTracking(TempestArmType armtype)
{
	if (GetArmStatus(armtype) == TempestArmStatus::ACTIVE)
	{
		SetArmStatus(armtype, TempestArmStatus::TRACKING);

		switch (armtype)
		{
		case TempestArmType::RIGHT:
		case TempestArmType::LEFT:
			if (ArmsTrackingTime > 0)
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, FName("OnTrackingDone"), armtype);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, ArmsTrackingTime, false);
			}
			else
			{
				OnTrackingDone(armtype);
			}
			break;
		case TempestArmType::SHIELD:
			if (ShieldTargetInRangeTime > 0)
			{
				FTimerDelegate TimerDel;
				TimerDel.BindUFunction(this, FName("OnTrackingDone"), armtype);
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, ArmsTrackingTime, false);
			}
			else
			{
				OnTrackingDone(armtype);
			}
			break;
		case TempestArmType::INVALID:
			break;

		}

		
	}
}

void UTempestGolemComponent::OnTrackingDone(TempestArmType armType)
{
	if (GetArmStatus(armType) == TempestArmStatus::TRACKING)
	{
		switch (armType)
		{
		case TempestArmType::RIGHT:
		case TempestArmType::LEFT:
			if ((armType == TempestArmType::LEFT && leftTargets.Num() > 0) || (armType == TempestArmType::RIGHT && rightTargets.Num() > 0))
			{
				SetArmStatus(armType, TempestArmStatus::LOCKED);
				if (ArmsLockedTime > 0)
				{
					FTimerDelegate TimerDel;
					TimerDel.BindUFunction(this, FName("ArmAttack"), armType);
					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, ArmsLockedTime, false);
				}
				else
				{
					ArmAttack(armType);
				}
			}
			else
			{
				SetArmStatus(armType, TempestArmStatus::ACTIVE);
			}
			break;
		case TempestArmType::SHIELD:
			if (shieldTargets.Num() > 0)
			{
				ArmAttack(armType);
			}
			else
			{
				SetArmStatus(armType, TempestArmStatus::ACTIVE);
			}
			break;
		case TempestArmType::INVALID:
			break;
		default:
			break;
		}
	}
}

float UTempestGolemComponent::GetLeftArmWeight()
{
	return FMath::Clamp(leftArmAttackWeight, 0.01f, 0.99f);
}

float UTempestGolemComponent::GetRightArmWeight()
{
	return FMath::Clamp(rightArmAttackWeight, 0.01f, 0.99f);
}

float UTempestGolemComponent::GetAttackSpeed()
{
	return swordArmsAttackSpeed;
}

void UTempestGolemComponent::SetAttackSpeed(float newSpeed)
{
	swordArmsAttackSpeed = newSpeed;
}


void UTempestGolemComponent::ApplyDamageReduction(float damageMultiplier)
{	
	if (abilitySystemComp) {
		RemoveDamageReduction();
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UDamageAbsorptionGameplayEffect>(abilitySystemComp, 1.0f);
		spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), 1.0f / damageMultiplier);
		damageReductionHandle = abilitySystemComp->ApplyGameplayEffectSpecToSelf(spec);
	}
};
void UTempestGolemComponent::RemoveDamageReduction()
{
	if (damageReductionHandle.IsValid() && abilitySystemComp) {
		abilitySystemComp->RemoveActiveGameplayEffect(damageReductionHandle);
		damageReductionHandle.Invalidate();
	}
};


void UTempestGolemComponent::DealDamage(TempestArmType armType)
{
	switch (armType)
	{
	case TempestArmType::LEFT:
		LeftArmAttackComponent->AttackLocal();
		break;
	case TempestArmType::RIGHT:
		RightArmAttackComponent->AttackLocal();
		break;
	case TempestArmType::SHIELD:
		ShieldAttackComponent->AttackLocal();
		break;
	}
}

FString UTempestGolemComponent::GetArmStatusText(TempestArmStatus ArmStatus)
{
	switch (ArmStatus)
	{
	case TempestArmStatus::ACTIVE:
		return "ACTIVE";
		break;
	case TempestArmStatus::ATTACKING:
		return "ATTACKING";
		break;
	case TempestArmStatus::DISABLED:
		return "DISABLED";
		break;
	case TempestArmStatus::DISABLING:
		return "DISABLING";
		break;
	case TempestArmStatus::REACTIVATING:
		return "REACTIVATING";
		break;
	case TempestArmStatus::INVALID:
		return "INVALID";
		break;
	default:
		break;
	}
	return "";
}

void UTempestGolemComponent::HandleDamageReceived(float amount)
{
	OnTempestGolemDamageReceived.Broadcast(amount);
	if (TempestGolemHealthComponent)
	{
		if (tempestStatus == TempestStatus::STUNNED)
		{
			float maxHealth = TempestGolemHealthComponent->GetMaximumHealth();
			percDamageReceivedWhileStunned += amount / maxHealth;
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Damage Received while stunned. Percentage = %f     PercentageReceived = %f"), amount / maxHealth, percDamageReceivedWhileStunned));

			if (HasReceivedEnoughDamageToUnstun())
			{
				// broadcast
				OnTempestGolemUnstunPercentageHit.Broadcast();
				percDamageReceivedWhileStunned = 0;
				SetTempestStatus(TempestStatus::ACTIVE);
			}
		}
	}
}

bool UTempestGolemComponent::HasReceivedEnoughDamageToUnstun()
{
	return percDamageReceivedWhileStunned >= RemoveStunAfterDamagePercentage;
}

void UTempestGolemComponent::OnDeath() const
{
	OnTempestGolemDeath.Broadcast();
}
