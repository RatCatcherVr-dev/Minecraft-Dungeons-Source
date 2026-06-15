#include "PlayfabServicesSettings.h"

UPlayfabServicesSettings::UPlayfabServicesSettings(const FObjectInitializer& initializer)
    : Super(initializer) {
    LoadConfig(UPlayfabServicesSettings::StaticClass());
}
