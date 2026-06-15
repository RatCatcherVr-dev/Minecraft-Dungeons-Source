#include "Dungeons.h"
#include "HealthDisplayComponent.h"

UHealthDisplayComponent::UHealthDisplayComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthDisplayComponent::OnAttributeHealthChange(const FOnAttributeChangeData& data) {
	const auto owner = GetOwner();	
	const auto fraction = data.NewValue / owner->FindComponentByClass<UHealthComponent>()->GetMaximumHealth();

	int index = 0;
	auto meshComponent = owner->FindComponentByClass<USkeletalMeshComponent>();

	for (auto&& material : meshComponent->GetMaterials()) {
		auto materialDynamic = Cast<UMaterialInstanceDynamic>(material);
		if (materialDynamic == nullptr) {
			materialDynamic = UMaterialInstanceDynamic::Create(material, this);
			meshComponent->SetMaterial(index, materialDynamic);
		}

		materialDynamic->SetScalarParameterValue(uniformName, fraction);
		index++;
	}
}
