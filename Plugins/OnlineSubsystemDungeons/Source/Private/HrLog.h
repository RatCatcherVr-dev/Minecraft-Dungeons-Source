#pragma once

#include <CoreMinimal.h>
#include "xsapiServicesInclude.h"

HRESULT HrLog(HRESULT, const FString& message);
HRESULT HrLogOnlyError(HRESULT, const FString& message);
bool HrFailed(HRESULT, const FString& message);
bool HrSuccess(HRESULT, const FString& message);
void HrCheck(HRESULT, const FString& message);
