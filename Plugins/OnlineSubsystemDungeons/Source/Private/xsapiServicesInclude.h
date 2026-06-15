#pragma once

#pragma warning(disable:4668) //C4668: 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives

#ifdef CPP
#undef CPP
#define XSAPI_CPP_UNDEFINED
#endif

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "AllowWindowsPlatformAtomics.h"
#include "AllowWindowsPlatformTypes.h"
#endif

#include "xsapi/services.h"

#if PLATFORM_WINDOWS
#include "HideWindowsPlatformTypes.h"
#include "HideWindowsPlatformAtomics.h"
#endif

#ifdef XSAPI_CPP_UNDEFINED
#define CPP 1
#undef XSAPI_CPP_UNDEFINED
#endif

#pragma warning(default:4668)