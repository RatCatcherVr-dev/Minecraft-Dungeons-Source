#include "Dungeons.h"
#include "MaterialLerpNotify.h"

bool UMaterialLerpNotify::Received_NotifyBegin(class USkeletalMeshComponent* meshComponent, class UAnimSequence* sequence, float totalDuration) const {
	auto material = meshComponent->GetMaterial(materialIndex);
	
	auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
	if (materialDynamic == nullptr) {
		materialDynamic = UMaterialInstanceDynamic::Create(material, meshComponent);
		meshComponent->SetMaterial(materialIndex, materialDynamic);
	}

	materialDynamic->SetScalarParameterValue(parameterTimeStartName, GetWorld()->GetTimeSeconds());
	materialDynamic->SetScalarParameterValue(parameterValueStartName, valueStart);
	materialDynamic->SetScalarParameterValue(parameterTimeEndName, GetWorld()->GetTimeSeconds() + totalDuration);
	materialDynamic->SetScalarParameterValue(parameterValueEndName, valueEnd);

	return true;
}