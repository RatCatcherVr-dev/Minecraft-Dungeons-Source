#include "PCHOnlineDungeonsSubsystem.h"
#include "HrLog.h"
#include "utils.h"
#include <OnlineSubsystem.h>

HRESULT HrLog(HRESULT hr, const FString& message) {
	if (SUCCEEDED(hr)) {
		UE_LOG_ONLINE(Log, TEXT("%s"), *message);
	} else {
		UE_LOG_ONLINE(Error, TEXT("%s, hr=%s (0x%x)"), *message, *ConvertHRtoString(hr), hr);
	}
	return hr;
}

HRESULT HrLogOnlyError(HRESULT hr, const FString& message) {
	if (FAILED(hr)) {
		HrLog(hr, message);
	}
	return hr;
}

bool HrFailed(HRESULT hr, const FString& message) {
	return FAILED(HrLog(hr, message));
}

bool HrSuccess(HRESULT hr, const FString& message) {
	return SUCCEEDED(HrLog(hr, message));
}

void HrCheck(HRESULT hr, const FString& message) {
	const auto res = HrSuccess(hr, message);
	check(res);
}

