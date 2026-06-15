#include "ComponentUtils.h"


namespace componentutils {


	UActorComponent* FindDefaultComponentByClass(const TSubclassOf<AActor> InActorClass, const TSubclassOf<UActorComponent> InComponentClass) {
		if (!IsValid(InActorClass))
		{
			return nullptr;
		}

		// Check CDO.
		AActor* ActorCDO = InActorClass->GetDefaultObject<AActor>();
		UActorComponent* FoundComponent = ActorCDO->FindComponentByClass(InComponentClass);

		if (FoundComponent != nullptr)
		{
			return FoundComponent;
		}

		// Check blueprint nodes. Components added in blueprint editor only (and not in code) are not available from
		// CDO.
		UBlueprintGeneratedClass* RootBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(InActorClass);
		UClass* ActorClass = InActorClass;

		// Go down the inheritance tree to find nodes that were added to parent blueprints of our blueprint graph.
		do
		{
			UBlueprintGeneratedClass* ActorBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(ActorClass);
			if (!ActorBlueprintGeneratedClass)
			{
				return nullptr;
			}

			const TArray<USCS_Node*>& ActorBlueprintNodes = ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes();

			for (USCS_Node* Node : ActorBlueprintNodes)
			{
				if (Node->ComponentClass->IsChildOf(InComponentClass))
				{
					return Node->GetActualComponentTemplate(RootBlueprintGeneratedClass);
				}
			}

			ActorClass = Cast<UClass>(ActorClass->GetSuperStruct());

		} while (ActorClass != AActor::StaticClass());

		return nullptr;
	}
}