
#include "DungeonsEditorModule.h"
#include "PropertyEditorModule.h"
#include "SerializableItemIdCustomization.h"
#include "EdGraphUtilities.h"
#include "SerializableIdGraphPanelPinFactory.h"

IMPLEMENT_GAME_MODULE(FDungeonsEditorModule, DungeonsEditor);

#define LOCTEXT_NAMESPACE "DungeonsEditor"
void FDungeonsEditorModule::StartupModule() {
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FDungeonsEditorModule::OnPostEngineInit);
}
void FDungeonsEditorModule::ShutdownModule() {
	if(FModuleManager::Get().IsModuleLoaded("PropertyEditor")) {
		auto& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout("FSerializableItemId");
	}	
}
void FDungeonsEditorModule::OnPostEngineInit() {
	auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	PropertyModule.RegisterCustomPropertyTypeLayout("SerializableItemId", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FSerializableItemIdCustomization::MakeInstance));
	PropertyModule.NotifyCustomizationModuleChanged();

	TSharedPtr<FSerializableIdGraphPanelPinFactory> GameplayTagsGraphPanelPinFactory = MakeShareable(new FSerializableIdGraphPanelPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(GameplayTagsGraphPanelPinFactory);
}

#undef LOCTEXT_NAMESPACE
