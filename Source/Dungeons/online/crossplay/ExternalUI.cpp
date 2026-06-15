#include "ExternalUI.h"
#include "SubsystemRepo.h"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "CrossplayOSS.h"
#include "DungeonsLoginFlow.h"
#include "DungeonsLoginFlowPS4.h"


namespace online {
namespace Crossplay {

namespace internal {
#define GUEST_ACCOUNT_IDENTIFIER "local_"

bool IsPS4GuestAccount(TSharedPtr<const FUniqueNetId> UniqueId) {
	return UniqueId->ToString().StartsWith(GUEST_ACCOUNT_IDENTIFIER);
}
}

ExternalUI::ExternalUI(const SubsystemRepo& subsystems) : SubOSS(subsystems) {
	for (auto* subsystem : GetSubsystemsWithExternalUI()) {
		subsystem->GetExternalUIInterface()->OnLoginFlowUIRequiredDelegates.AddLambda([this](const FString& RequestedURL, const FOnLoginRedirectURL& OnLoginRedirect, const FOnLoginFlowComplete& OnLoginFlowComplete, bool& bOutShouldContinueLogin) {
			if (this->OnLoginFlowUIRequiredDelegates.IsBound()) {
				this->OnLoginFlowUIRequiredDelegates.Broadcast(RequestedURL, OnLoginRedirect, OnLoginFlowComplete, bOutShouldContinueLogin);
			}
		});
	}
}

auto ExternalUI::CreateAccountLinkingLambda(const FOnLoginUIClosedDelegate& Delegate, const int ControllerIndex, TSharedPtr<const FUniqueNetId> UniqueId, const IOnlineSubsystem* subsystem) {
	return [this, Delegate, ControllerIndex, UniqueId, subsystem](AccountLinkStatus currentStatus) {
		this->DoAccountLinking(UniqueId, ControllerIndex, Delegate, subsystem, currentStatus);
	};
}

auto ExternalUI::LinkAccounts(const FOnLoginUIClosedDelegate& Delegate, const int ControllerIndex, TSharedPtr<const FUniqueNetId> UniqueXblId, const IOnlineSubsystem* subsystem) {
#if defined(HAS_ON_AUTH_TOKEN_UPDATE) && defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS) && !WITH_EDITOR 
	subsystem->GetIdentityInterface()->GetAuthTokenAsync("playfab_key", [this, Delegate, ControllerIndex, UniqueXblId, subsystem] (FString xblToken, FString issuerIdXBL) {
		GetPS4Subsystem()->GetIdentityInterface()->GetAuthTokenAsync("playfab_key", [this, Delegate, ControllerIndex, UniqueXblId, subsystem, xblToken, issuerIdXBL](FString psnToken, FString issuerIdPSN) {
			if (!PlayfabServices::LinkAccounts({ psnToken, xblToken, issuerIdPSN, UniqueXblId->ToString(), CreateAccountLinkingLambda(Delegate, ControllerIndex, UniqueXblId, subsystem) })) {
				TriggerXblLoginError(UniqueXblId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::GenericLinkingError));
			}
		});
	});
#endif
}

void ExternalUI::SetGamertagHint(const int ControllerIndex) {
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS) && !WITH_EDITOR 
	GetPS4Subsystem()->GetIdentityInterface()->GetAuthTokenAsync("playfab_key", [ControllerIndex](FString psnToken, FString issuerIdPSN) {
		auto callback = [ControllerIndex](bool success) {
			if (success) {
				FString XBLGamerTagHint = online::getIdentityInterface()->GetSecondaryPlayerNickname(ControllerIndex);
				if (!XBLGamerTagHint.IsEmpty())
					PlayfabServices::SetGamertagHint(XBLGamerTagHint);
			}
		};
		PlayfabServices::RegisterOnAuthenticationCallback({psnToken, issuerIdPSN, callback});
	});
#endif
}

void ExternalUI::DoAccountLinking(TSharedPtr<const FUniqueNetId> UniqueXblId,
								  const int ControllerIndex,
								  const FOnLoginUIClosedDelegate& Delegate,
								  const IOnlineSubsystem* subsystem,
								  AccountLinkStatus currentStatus) {
	switch(currentStatus) {
		case AccountLinkStatus::NotLinked: {
			if (const auto DGameInstance = GetWorldForOnline("Dungeons")->GetGameInstance<UDungeonsGameInstance>()) {
				auto loginFlow = Cast<UDungeonsLoginFlowPS4>(DGameInstance->GetLoginFlow());
				if (loginFlow->IsLinkingShown()) {
					LinkAccounts(Delegate, ControllerIndex, UniqueXblId, subsystem);
				} else {
					TriggerXblLoginError(UniqueXblId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::GenericLinkingError));
				}
			} else {
				TriggerXblLoginError(UniqueXblId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::GenericLinkingError));
			}
			break;
		}
		case AccountLinkStatus::Linked: {
			if (auto* world = GetWorldForOnline(GetDungeonsSubsystem()->GetInstanceName())) {
				auto pc = Cast<APlayerControllerBase>(world->GetFirstPlayerController());
				online::SetXblActive(true, pc);
			}
			SetGamertagHint(ControllerIndex);
			Delegate.ExecuteIfBound(UniqueXblId, ControllerIndex, FOnlineError(true));
			break;
		}
		case AccountLinkStatus::ThisPsnAlreadyLinkedToDifferentMSA: {
			// TODO: the unlinking cloud script should copy over the GamertagHint to the unlinked PF(PSN) account
			auto gamertagHintCallback = [this, UniqueXblId, ControllerIndex, Delegate](FString gameTagHint) {
				this->LinkErrorGamertagHint = gameTagHint;
				UE_LOG(LogOnline, Warning, TEXT("[ExternalUI] PSN Account already linked to another XBL account: %s"), *gameTagHint);
				this->TriggerXblLoginError(UniqueXblId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::PSNLinkedToDifferentMSAError));
			};
			PlayfabServices::GetGamertagHint(gamertagHintCallback);
			break;
		}
		case AccountLinkStatus::ThisMsaAlreadyLinkedToDifferentPSN: {
			TriggerXblLoginError(UniqueXblId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::MSALinkedToDifferentPSNError));
			break;
		}
		case AccountLinkStatus::Error:
		default:
			TriggerXblLoginError(UniqueXblId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::GenericLinkingError));
			break;
	}
}

auto ExternalUI::CreateXblLoginLambda(const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, TSharedPtr<const FUniqueNetId> UniquePS4Id) {
	return [this, Delegate, subsystem, UniquePS4Id] (TSharedPtr<const FUniqueNetId> UniqueXblId, int ControllerIndex, const FOnlineError& Error) {
		if (Error.bSucceeded) {
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS)
			subsystem->GetIdentityInterface()->AddAuthToken("playfab_key", "POST", "https://playfabapi.com/");
#endif
			DoAccountLinking(UniqueXblId, ControllerIndex, Delegate, subsystem, PlayfabServices::GetAccountLinkStatus(UniqueXblId->ToString()));
		} else {
			TriggerXblLoginError(UniquePS4Id, ControllerIndex, Delegate, Error);
		}
	};
}

auto ExternalUI::CreatePlayfabLoginLambda(const FOnLoginUIClosedDelegate& Delegate, TSharedPtr<const FUniqueNetId> PS4UniqueId, const IOnlineSubsystem* subsystem, const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton) {
	return [this, Delegate, PS4UniqueId, subsystem, ControllerIndex, bShowOnlineOnly, bShowSkipButton] (bool success) {
		if (success) {
			FOnLoginUIClosedDelegate child;
			child.BindLambda(CreateXblLoginLambda(Delegate, subsystem, PS4UniqueId));
			subsystem->GetExternalUIInterface()->ShowLoginUI(ControllerIndex, bShowOnlineOnly, bShowSkipButton, child);
		} else {
			TriggerXblLoginError(PS4UniqueId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::FailedPlayfabLoginError));
		}
	};
}

auto ExternalUI::DoPlayfabLogin(const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, bool bShowOnlineOnly, bool bShowSkipButton, TSharedPtr<const FUniqueNetId> UniquePS4Id, const int ControllerIndex, const FOnlineError& Error) {
	
	auto* world = GetWorldForOnline(GetDungeonsSubsystem()->GetInstanceName());
	if (world) {
		auto pc = Cast<APlayerControllerBase>(world->GetFirstPlayerController());
		online::SetXblActive(false, pc);
	}
	if (subsystem && Error.bSucceeded) {
		if(const auto DGameInstance = GetWorldForOnline(subsystem->GetInstanceName())->GetGameInstance<UDungeonsGameInstance>()) {
#if defined(SUBSYSTEM_HAS_AUTH_TOKEN_FUNCTIONS) && !WITH_EDITOR 
			//D11.PS added this timeout to handle an error if we hear nothing back from the sign in within 30 seconds. 
			//It might be too long, and this isnt the ideal fix but its the safest fix at this late stage.
			TimedOut = false;
			world->GetTimerManager().SetTimer(TimeOutHandle, [this, Delegate, UniquePS4Id, ControllerIndex] {
				TimedOut = true;
				TriggerXblLoginError(UniquePS4Id, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::NetworkIssueError));
			}, 30.0f, false);

			auto player = DGameInstance->GetFirstGamePlayer();
			GetPS4Subsystem()->GetIdentityInterface()->GetAuthTokenAsync("playfab_key", [this, Delegate, UniquePS4Id, subsystem, ControllerIndex, bShowOnlineOnly, bShowSkipButton, world] (FString token, FString issuerId) {
				if (!TimedOut)
				{
					auto callback = CreatePlayfabLoginLambda(Delegate, UniquePS4Id, subsystem, ControllerIndex, bShowOnlineOnly, bShowSkipButton);
					world->GetTimerManager().ClearTimer(TimeOutHandle);
					if (!PlayfabServices::RegisterOnAuthenticationCallback({ token, issuerId, callback })) {
						TriggerXblLoginError(UniquePS4Id, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::FailedPlayfabLoginError));
					}
				}
			});
#endif
		} else {
			TriggerXblLoginError(UniquePS4Id, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::FailedMSALoginError));
		}
	} else {
		TriggerXblLoginError(UniquePS4Id, ControllerIndex, Delegate, Error);
	}
}

auto ExternalUI::CreatePSNLoginLambda(const FOnLoginUIClosedDelegate& Delegate, const IOnlineSubsystem* subsystem, bool bShowOnlineOnly, bool bShowSkipButton) {
	return [this, Delegate, subsystem, bShowOnlineOnly, bShowSkipButton]
	(TSharedPtr<const FUniqueNetId> UniqueId, int ControllerIndex, const FOnlineError& Error) {
		if (Error.bSucceeded) {
			if(const auto DGameInstance = GetWorldForOnline(subsystem->GetInstanceName())->GetGameInstance<UDungeonsGameInstance>()) {
				if (DGameInstance->CheckConnectionStatus()) {
					if (!internal::IsPS4GuestAccount(UniqueId)) {
						DoPlayfabLogin(Delegate, subsystem, bShowOnlineOnly, bShowSkipButton, UniqueId, ControllerIndex, Error);
					} else {
						TriggerXblLoginError(UniqueId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::GuestAccountError));
					}
				} else {
					TriggerXblLoginError(UniqueId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::NetworkIssueError));
				}
			} else {
				TriggerXblLoginError(UniqueId, ControllerIndex, Delegate, FOnlineError(LoginFlowConstants::FailedMSALoginError));
			}
		} else {
			TriggerXblLoginError(UniqueId, ControllerIndex, Delegate, Error);
		}
	};
}

bool ExternalUI::CrossplayLoginFlow(const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate) {
	FOnLoginUIClosedDelegate child;
	child.BindLambda(CreatePSNLoginLambda(Delegate, GetDungeonsSubsystem(), bShowOnlineOnly, bShowSkipButton));
	return GetPS4Subsystem()->GetExternalUIInterface()->ShowLoginUI(ControllerIndex, bShowOnlineOnly, bShowSkipButton, child);
}

bool ExternalUI::ShowLoginUI(const int ControllerIndex, bool bShowOnlineOnly, bool bShowSkipButton, const FOnLoginUIClosedDelegate& Delegate) {
	if (Subsystems.IsActive(SubsystemType::PS4) && Subsystems.IsActive(SubsystemType::Dungeons)) {
		return CrossplayLoginFlow(ControllerIndex, bShowOnlineOnly, bShowSkipButton, Delegate);
	}

	if (const auto* subsystem = GetFirstActiveSubsystem({ SubsystemType::Dungeons, SubsystemType::PS4 })) {
		return subsystem->GetExternalUIInterface()->ShowLoginUI(ControllerIndex, bShowOnlineOnly, bShowSkipButton, Delegate);
	}
	return false;
}

void ExternalUI::TriggerXblLoginError(TSharedPtr<const FUniqueNetId> UniqueId, const int ControllerIndex, const FOnLoginUIClosedDelegate& Delegate, FOnlineError Error) {
	Delegate.ExecuteIfBound(UniqueId, ControllerIndex, Error);
}

bool ExternalUI::ShowAccountUpgradeUI(const FUniqueNetId& UniqueId) {
	if (const auto* subsystem = GetSubsystemFromUniqueNetId(UniqueId)) {
		return subsystem->GetExternalUIInterface()->ShowAccountUpgradeUI(UniqueId);
	}
	return false;
}

bool ExternalUI::ShowProfileUI(const FUniqueNetId& Requestor, const FUniqueNetId& Requestee, const FOnProfileUIClosedDelegate& Delegate) {
	if (const auto* subsystem = GetFirstActiveSubsystem({ SubsystemType::PS4, SubsystemType::Dungeons })) {
		return subsystem->GetExternalUIInterface()->ShowProfileUI(*GetUniqueNetIdFromName(Requestor, Requestee.GetType()).Get(), Requestee, Delegate);
	}
	return false;
}

bool ExternalUI::ShowStoreUI(int32 LocalUserNum, const FShowStoreParams& ShowParams, const FOnShowStoreUIClosedDelegate& Delegate) {
	if (const auto* subsystem = GetFirstActiveSubsystem({ SubsystemType::PS4, SubsystemType::Steam, SubsystemType::Dungeons })) {
		return subsystem->GetExternalUIInterface()->ShowStoreUI(LocalUserNum, ShowParams, Delegate);
	}
	return false;
}

void ExternalUI::ShowStoreUI(UWorld* World, int32 LocalUserNum, const FString category, FString productID) {
	FShowStoreParams storeParams;
	storeParams.Category = category;
	storeParams.ProductId = productID;
	ShowStoreUI(LocalUserNum, storeParams);
}

TArray<IOnlineSubsystem*> ExternalUI::GetSubsystemsWithExternalUI() const {
	return Subsystems.GetAllActive().FilterByPredicate([](const IOnlineSubsystem* subsystem) {
		return subsystem->GetExternalUIInterface().IsValid();
	});
}

FString ExternalUI::GetLinkErrorGamertagHint() const {
	return LinkErrorGamertagHint;
}

}
}
