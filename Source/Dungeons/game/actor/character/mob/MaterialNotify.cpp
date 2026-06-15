#include "Dungeons.h"
#include "MaterialNotify.h"

bool UMaterialNotify::Received_Notify(class USkeletalMeshComponent* meshComponent, class UAnimSequence* sequence) const {
	auto material = meshComponent->GetMaterial(materialIndex);
	
	auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
	if (materialDynamic == nullptr) {
		materialDynamic = UMaterialInstanceDynamic::Create(material, meshComponent);
		meshComponent->SetMaterial(materialIndex, materialDynamic);
	}

	materialDynamic->SetScalarParameterValue(parameterName, value);

	return true;
}