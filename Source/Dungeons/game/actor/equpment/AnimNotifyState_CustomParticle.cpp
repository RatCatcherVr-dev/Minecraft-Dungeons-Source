#include "Dungeons.h"
#include "AnimNotifyState_CustomParticle.h"
#include "Assets/DungeonsAssetManager.h"
#include "DungeonsGameInstance.h"

const FName UAnimNotifyState_CustomParticle::COLOR_PARAMETER = FName("Color");
const FName UAnimNotifyState_CustomParticle::LIFETIME_PARAMETER = FName("Lifetime");
const FName UAnimNotifyState_CustomParticle::ROTATION_SPEED_PARAMETER = FName("RotationSpeed");

UAnimNotifyState_CustomParticle::UAnimNotifyState_CustomParticle()
	: Super()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(150, 220, 255);
#endif // WITH_EDITORONLY_DATA
}

UAnimNotifyState_CustomParticle::UAnimNotifyState_CustomParticle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PSTemplate = nullptr;
	LocationOffset.Set(0.0f, 0.0f, 0.0f);
	RotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	Scale = FVector(1.0f, 1.0f, 1.0f);
	Color = FColor::White;
	Lifetime = 0.5f;
	RotationSpeed = 0.5f;

	#if WITH_EDITORONLY_DATA
		NotifyColor = FColor(150, 220, 255);
	#endif // WITH_EDITORONLY_DATA
}

void UAnimNotifyState_CustomParticle::PostLoad() {
	Super::PostLoad();

	RotationOffsetQuat = FQuat(RotationOffset);
}

#if WITH_EDITOR
void UAnimNotifyState_CustomParticle::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty && PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UAnimNotifyState_CustomParticle, RotationOffset)) {
		RotationOffsetQuat = FQuat(RotationOffset);
	}
}
#endif

void UAnimNotifyState_CustomParticle::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) {
	// Don't call super to avoid unnecessary call in to blueprints
	if (PSTemplate) 
	{
		if (PSTemplate->IsImmortal()) {
			UE_LOG(LogTemp, Warning, TEXT("Particle Notify: Anim %s tried to spawn infinitely looping particle system %s. Spawning suppressed."), *GetNameSafe(Animation), *GetNameSafe(PSTemplate));
			return;
		}
		const FTransform MeshTransform = MeshComp->GetSocketTransform(FName());
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(MeshTransform.TransformPosition(LocationOffset));
		SpawnTransform.SetRotation(MeshTransform.GetRotation() * RotationOffsetQuat);
		auto emitter = UGameplayStatics::SpawnEmitterAtLocation(MeshComp->GetWorld(), PSTemplate, SpawnTransform);
		emitter->SetRelativeScale3D(Scale);
		emitter->SetColorParameter(COLOR_PARAMETER, Color);
		emitter->SetFloatParameter(LIFETIME_PARAMETER, Lifetime);
		emitter->SetFloatParameter(ROTATION_SPEED_PARAMETER, RotationSpeed);
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Particle Notify: Null PSTemplate for particle notify in anim: %s"), *GetNameSafe(Animation));
	}
}

void UAnimNotifyState_CustomParticle::NotifyTick(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float FrameDeltaTime) {
}

void UAnimNotifyState_CustomParticle::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) {
}

FString UAnimNotifyState_CustomParticle::GetNotifyName_Implementation() const {
	if (PSTemplate) {
		return PSTemplate->GetName();
	} else {
		return Super::GetNotifyName_Implementation();
	}
}
