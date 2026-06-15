// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DungeonsAbilitySystemGlobals.h"
#include "effects/GameplayEffectUtil.h"
#include <GameplayCueManager.h>
#include "game/actor/character/BaseCharacter.h"

void FDungeonsGameplayEffectContext::AddInstigator(class AActor *InInstigator, class AActor *InEffectCauser) {
	Super::AddInstigator(InInstigator, InEffectCauser);
	if (auto character = Cast<ABaseCharacter>(InInstigator)) {
		InstigatorTeam = character->GetCurrentTeam();
	}
}

float UDungeonsAbilitySystemGlobals::GetPeriod(const FGameplayEffectContextHandle& handle)
{
	if(const auto context = effects::GetDungeonsEffectContextFromHandle(handle)) {
		return context->Period.Get(0.f);
	}

	return 1.f;
}

void UDungeonsAbilitySystemGlobals::PushParameterFilterContext(FGameplayCueParametersFilter filter) {
	ParamFilterStack.Emplace(filter);
	GetGameplayCueManager()->StartGameplayCueSendContext();
}
void UDungeonsAbilitySystemGlobals::PopParameterFilterContext() {
#if WITH_EDITOR
	ensureMsgf(ParamFilterStack.Num() > 0, TEXT("Attempting to pop GameplayCueParamFilter when stack is empty!"));
#endif

	GetGameplayCueManager()->EndGameplayCueSendContext();
	ParamFilterStack.Pop();
}

void UDungeonsAbilitySystemGlobals::GlobalPreGameplayEffectSpecApply(FGameplayEffectSpec& Spec, UAbilitySystemComponent* AbilitySystemComponent) {
	FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);

	if (!context->Normal.IsSet() && AbilitySystemComponent->GetOwner()) {
		effects::StorePushbackInNormal(Spec, (AbilitySystemComponent->GetOwner()->GetActorLocation() - context->GetOrigin()).GetSafeNormal());
	}

	if (!CachedGameBP.IsValid()) {
		CachedGameBP = actorquery::getFirstActor<AGameBP>(AbilitySystemComponent->GetWorld());
	}

	if(Spec.Def->DurationPolicy != EGameplayEffectDurationType::Instant && Spec.GetPeriod() != 0.f)
	{
		context->Period = Spec.GetPeriod();
	}

	context->Game = CachedGameBP.IsValid() ? CachedGameBP->GetGame() : nullptr;
}

void UDungeonsAbilitySystemGlobals::InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectContextHandle& EffectContext) {
	if (!EffectContext.IsValid()) return;

	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsEffectContextFromHandle(EffectContext);

	bool sendNormal = context->Normal.IsSet();
	
	if(ParamFilterStack.Num() > 0) {
		const auto filter = ParamFilterStack.Last();
		if(filter & EGameplayCueParametersField::Instigator) CueParameters.Instigator = EffectContext.GetInstigator();
		if(filter & EGameplayCueParametersField::EffectCauser) CueParameters.EffectCauser = EffectContext.GetEffectCauser();
		if(filter & EGameplayCueParametersField::Location) CueParameters.Location = EffectContext.GetOrigin();
		if(filter & EGameplayCueParametersField::SourceObject) CueParameters.SourceObject = EffectContext.GetSourceObject();

		if((filter & EGameplayCueParametersField::Normal) && sendNormal) {
			CueParameters.Normal = context->Normal.GetValue();
		}
	} else {		
		CueParameters.Instigator = EffectContext.GetInstigator();
		CueParameters.EffectCauser = EffectContext.GetEffectCauser();
		CueParameters.Location = EffectContext.GetOrigin();
		CueParameters.SourceObject = EffectContext.GetSourceObject();
		if(sendNormal) CueParameters.Normal = context->Normal.GetValue();
	}

	//Dot Execute hack.
	if(context->Period.IsSet()) {
		FGameplayEffectContextHandle dummyHandle(AllocGameplayEffectContext());
		FDungeonsGameplayEffectContext* dummyContext = effects::GetDungeonsEffectContextFromHandle(dummyHandle);
		dummyContext->Period = context->Period;	
		CueParameters.EffectContext = dummyHandle;
	}
}

void UDungeonsAbilitySystemGlobals::InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectSpecForRPC& Spec) {
	if(ParamFilterStack.Num() > 0) {
		const auto filter = ParamFilterStack.Last();
		if(filter & EGameplayCueParametersField::AggregatedSourceTags) CueParameters.AggregatedSourceTags = Spec.AggregatedSourceTags;
		if(filter & EGameplayCueParametersField::AggregatedTargetTags) CueParameters.AggregatedTargetTags = Spec.AggregatedTargetTags;
		if(filter & EGameplayCueParametersField::GELevel) CueParameters.GameplayEffectLevel = Spec.GetLevel();
	} else {
		CueParameters.AggregatedSourceTags = Spec.AggregatedSourceTags;
		CueParameters.AggregatedTargetTags = Spec.AggregatedTargetTags;
		CueParameters.GameplayEffectLevel = Spec.GetLevel();
	}
	
	InitGameplayCueParameters(CueParameters, Spec.GetContext());
}

void UDungeonsAbilitySystemGlobals::InitGameplayCueParameters_GESpec(FGameplayCueParameters& CueParameters, const FGameplayEffectSpec& Spec) {

	if(ParamFilterStack.Num() > 0) {
		const auto filter = ParamFilterStack.Last();
		if(filter & EGameplayCueParametersField::AggregatedSourceTags) CueParameters.AggregatedSourceTags = *Spec.CapturedSourceTags.GetAggregatedTags();
		if(filter & EGameplayCueParametersField::AggregatedTargetTags) CueParameters.AggregatedTargetTags = *Spec.CapturedTargetTags.GetAggregatedTags();

		if((filter & EGameplayCueParametersField::RawMagnitude) || (filter & EGameplayCueParametersField::NormalizedMagnitude)) {
			for (const FGameplayEffectCue& CueDef : Spec.Def->GameplayCues)
			{	
				bool FoundMatch = false;
				if (CueDef.MagnitudeAttribute.IsValid())
				{
					for (const FGameplayEffectModifiedAttribute& ModifiedAttribute : Spec.ModifiedAttributes)
					{
						if (ModifiedAttribute.Attribute == CueDef.MagnitudeAttribute)
						{
							CueParameters.RawMagnitude = ModifiedAttribute.TotalMagnitude;
							FoundMatch = true;
							break;
						}
					}
					if (FoundMatch)
					{
						break;
					}
				}
			}
		}

		if(filter & EGameplayCueParametersField::GELevel) CueParameters.GameplayEffectLevel = Spec.GetLevel();
		if(filter & EGameplayCueParametersField::AbilityLevel) CueParameters.AbilityLevel = Spec.GetEffectContext().GetAbilityLevel();		
		
	} else {
		CueParameters.AggregatedSourceTags = *Spec.CapturedSourceTags.GetAggregatedTags();
		CueParameters.AggregatedTargetTags = *Spec.CapturedTargetTags.GetAggregatedTags();

		// Look for a modified attribute magnitude to pass to the CueParameters
		for (const FGameplayEffectCue& CueDef : Spec.Def->GameplayCues)
		{	
			bool FoundMatch = false;
			if (CueDef.MagnitudeAttribute.IsValid())
			{
				for (const FGameplayEffectModifiedAttribute& ModifiedAttribute : Spec.ModifiedAttributes)
				{
					if (ModifiedAttribute.Attribute == CueDef.MagnitudeAttribute)
					{
						CueParameters.RawMagnitude = ModifiedAttribute.TotalMagnitude;
						FoundMatch = true;
						break;
					}
				}
				if (FoundMatch)
				{
					break;
				}
			}
		}

		CueParameters.GameplayEffectLevel = Spec.GetLevel();
	}

	InitGameplayCueParameters(CueParameters, Spec.GetContext());	
}

FGameplayEffectContext* UDungeonsAbilitySystemGlobals::AllocGameplayEffectContext() const {
	return new FDungeonsGameplayEffectContext();
}

bool FDungeonsGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) {
	uint16 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (Normal.IsSet()) {
			RepBits |= 1 << 7;
		}
		if(Period.IsSet()) {
			RepBits |= 1 << 8;
		}
		if (InstigatorTeam != ETeamName::World) {
			RepBits |= 1 << 9;
		}
	}

	Ar.SerializeBits(&RepBits, 10);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7)) {
		FVector tmp(Normal.IsSet() ? Normal.GetValue() : FVector::ZeroVector);
		Ar << tmp;
		Normal = tmp;
	}
	if(RepBits & (1 << 8)) {
		if(Ar.IsLoading()) {
			float tmp;
			ReadFixedCompressedFloat<1, 10>( tmp, Ar);
			Period = tmp;
		} else {
			WriteFixedCompressedFloat<1, 10>( Period.GetValue(), Ar);
		}
	}

	if (RepBits & (1 << 9)) {
		Ar << InstigatorTeam;
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}
	
	bOutSuccess = true;
	return true;
}


FParameterFilterContextWindow::FParameterFilterContextWindow()
: Globals(*Cast<UDungeonsAbilitySystemGlobals>(&UAbilitySystemGlobals::Get())) {
	Globals.PushParameterFilterContext(0);
}

FParameterFilterContextWindow::FParameterFilterContextWindow(FGameplayCueParametersFilter filter)
: Globals(*Cast<UDungeonsAbilitySystemGlobals>(&UAbilitySystemGlobals::Get())) {
	Globals.PushParameterFilterContext(filter);
}

FParameterFilterContextWindow::FParameterFilterContextWindow(EGameplayCueParametersField filter)
	: Globals(*Cast<UDungeonsAbilitySystemGlobals>(&UAbilitySystemGlobals::Get())) {
	Globals.PushParameterFilterContext(static_cast<FGameplayCueParametersFilter>(filter));
}

FParameterFilterContextWindow::~FParameterFilterContextWindow() {
	Globals.PopParameterFilterContext();
}


FGameplayEffectContext* FDungeonsGameplayEffectContext::Duplicate() const
{
	FDungeonsGameplayEffectContext* NewContext = new FDungeonsGameplayEffectContext();
	*NewContext = *this;
	NewContext->AddActors(Actors);
	if (GetHitResult())
	{
		// Does a deep copy of the hit result
		NewContext->AddHitResult(*GetHitResult(), true);
	}
	return NewContext;
}