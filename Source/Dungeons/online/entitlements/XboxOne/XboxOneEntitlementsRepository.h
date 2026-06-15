#pragma once

#include "CoreMinimal.h"
#include "../EntitlementsRepository.h"
#include "XboxOneEntitlementsRepository.generated.h"


typedef Windows::Foundation::TypedEventHandler<Windows::Xbox::Management::Deployment::IDownloadableContentPackage^, Windows::Xbox::Management::Deployment::ILicenseTerminatedEventArgs^>  Xb1LicenseTerminatedEventHandler;
typedef Windows::Foundation::EventHandler<Windows::Xbox::System::SignInCompletedEventArgs^> Xb1SignInCompletedEventHandler;

UCLASS()
class DUNGEONS_API UXboxOneEntitlementsRepository : public UEntitlementsRepository {

	GENERATED_BODY()
public:	
	void RequestEntitlements() override;
protected:
	void ConfigureForPlatform() override;
private:
	void RegisterPackageEvents(Windows::Xbox::Management::Deployment::IDownloadableContentPackage ^Package);
	void UnregisterPackageEvents(Windows::Xbox::Management::Deployment::IDownloadableContentPackage ^Package);

	Windows::Xbox::Management::Deployment::IDownloadableContentPackageManager^	PackageManager;
	TMap<FString, Windows::Foundation::EventRegistrationToken>					PackageEvents;
	Xb1LicenseTerminatedEventHandler^											LicenseTerminatedHandler;
	Xb1SignInCompletedEventHandler^												SignInCompletedHandler;
};
