#include "Dungeons.h"
#include "XboxOneEntitlementsRepository.h"

#include <collection.h>

using namespace Windows::Xbox::Management::Deployment;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;
using namespace Windows::Xbox::System;

void UXboxOneEntitlementsRepository::ConfigureForPlatform()
{
	ensure(GetSource() == EEntitlementsSource::XBL);
	
	PackageManager = ref new DownloadableContentPackageManager();
	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UXboxOneEntitlementsRepository::RequestEntitlements);
	//Refresh the entitlements when a user signs in or when a license is terminated
	LicenseTerminatedHandler = ref new Xb1LicenseTerminatedEventHandler(
		[this](IDownloadableContentPackage^ package, ILicenseTerminatedEventArgs^ args)
	{
		RequestEntitlements();
	});

	SignInCompletedHandler = ref new Xb1SignInCompletedEventHandler(
		[this](Platform::Object^, SignInCompletedEventArgs^ EventArgs)
	{
		RequestEntitlements();
	});

	PackageManager->DownloadableContentPackageInstallCompleted += ref new DownloadableContentPackageInstallCompletedEventHandler(
		[this]()
	{
		RequestEntitlements();
	});

	FCoreDelegates::ApplicationHasEnteredForegroundDelegate.AddUObject(this, &UXboxOneEntitlementsRepository::RequestEntitlements);
}

void UXboxOneEntitlementsRepository::RequestEntitlements()
{
	RemoveAllEntitlements();
	//D11.PS - Add the base game entitlement
	AddEntitlementsForPlatform("game_dungeons", GetSource());

	try
	{
		auto AllPackages = PackageManager->FindPackages(InstalledPackagesFilter::AllDownloadableContentOnly);

		for (auto Package : AllPackages)
		{
			bool Trial = false;
			if (Package->CheckLicense(&Trial))
			{
				FString PackageName = Package->PackageFullName->Data();
				AddEntitlementsForPlatform(PackageName, GetSource());
				RegisterPackageEvents(Package);
			}
		}

	}
	catch (Platform::Exception ^)
	{
		UE_LOG(LogDungeonsEntitlements, Warning, TEXT("UXboxOneEntitlementsRepository: Failed to refresh DLC packages."));
	}
	
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		OnEntitlementsProvided.Broadcast(GetEntitlements());
	});
}

void UXboxOneEntitlementsRepository::RegisterPackageEvents(IDownloadableContentPackage ^Package)
{
	if (!PackageEvents.Contains(Package->PackageFullName->Data()))
	{
		Windows::Foundation::EventRegistrationToken token = Package->LicenseTerminated += LicenseTerminatedHandler;
		PackageEvents.Add(Package->PackageFullName->Data(), token);
	}
}

void UXboxOneEntitlementsRepository::UnregisterPackageEvents(IDownloadableContentPackage ^Package)
{
	Windows::Foundation::EventRegistrationToken* Token = PackageEvents.Find(Package->PackageFullName->Data());

	if (Token != nullptr)
	{
		Package->LicenseTerminated -= *Token;
		PackageEvents.Remove(Package->PackageFullName->Data());
	}
}

