#pragma once

#pragma warning(disable:4668)
#pragma warning(disable:4583)

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include "AllowWindowsPlatformAtomics.h"
#endif
#pragma warning(disable: 4596)
#pragma warning(disable:4583)
#pragma warning(disable:4582)

#define NOMINMAX
#include "api/peer_connection_interface.h"
#include "api/data_channel_interface.h"
#include "pc/peer_connection_factory.h"
//#include "api/mediaconstraintsinterface.h"
#include "rtc_base/message_queue.h"
#include "rtc_base/message_handler.h"
#include "rtc_base/ssl_adapter.h"
#include "rtc_base/physical_socket_server.h"

#if PLATFORM_WINDOWS
#include "HideWindowsPlatformAtomics.h"
#endif
#pragma warning(default: 4596)
#pragma warning(default:4583)
#pragma warning(default:4582)

#if PLATFORM_SWITCH
#define WINAPI
typedef unsigned long DWORD;
#endif

#undef SendMessage
#pragma warning(default:4668)
#pragma warning(default:4583)