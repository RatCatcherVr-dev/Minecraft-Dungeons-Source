#pragma once

//#include "BuildConfig.h"
#include "legacy/Core/pch_core.h"

//#ifndef MCPE_HEADLESS
//#include "Renderer/pch_renderer.h"
//#endif

//do not warn for library files
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wshadow"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wall"
#pragma clang diagnostic ignored "-Wshadow"
#endif

#ifdef WIN32
#else
#include <errno.h>
#include <unistd.h>
#endif

#if defined(LINUX) || defined(ANDROID) || defined(__APPLE__) || defined(POSIX)
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#endif
#ifdef MACOSX
#include <CoreServices/CoreServices.h>
#include <unistd.h>
#endif

#ifdef ANDROID
#include <android/log.h>
namespace std {
	template <typename T>
	std::string to_string(const T& value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
}

#endif

#ifdef __cplusplus

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <set>
#include <unordered_set>
#include <chrono>
#include <unordered_map>
#include <map>
#include <list>
#include <utility>
#include <sstream>
#include <thread>
#include <limits>
#include <mutex>
#include <queue>
#include <deque>
#include <condition_variable>
#include <fstream>
#include <atomic>
#include <array>
#include <bitset>
#include <cmath>
#include <limits>
#include <bitset>
#include <future>

#include <cassert>
#include <cctype>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <cstdlib>

#include <json/json.h>
#include <json/writer.h>

#ifdef WIN32
#include "AllowWindowsPlatformTypes.h"
#include "HideWindowsPlatformTypes.h"
#endif

#include "util/Math.h"
#include "util/StringUtils.h"
#include "util/StringHash.h"

#include "legacy/Core/Utility/EnumCast.h"
#include "legacy/Core/Platform/ErrorHandling.h"

#endif

template <typename EnumType>
struct enum_name_tag {};

#define ENUM_NAME(EnumType) template<> struct enum_name_tag<EnumType> { constexpr static auto value = TEXT(#EnumType); };

#define USTRUCT_FORCEINIT_CTOR(T) template<> struct TStructOpsTypeTraits<T>: public TStructOpsTypeTraitsBase2<T> { enum { WithNoInitConstructor = true }; };

#define RETLAMBDA(body) [&](const auto& it) { return body; }
//D11.PS removed unused &
#define RETLAMBDA0(body) [&] { return body; }
#define RETLAMBDA1(body) [](const auto& it) { return body; }
#define RETLAMBDA2(body) [] { return body; }

#include <zlib/zlib.h>

#pragma warning(disable:4068) //allow unknown pragmas
#pragma warning(disable:5038) /* data member 'member1' will be initialized after data member 'member2' data member 'member' will be initialized after base class 'base_class' */
//#pragma warning(disable)

#ifndef _MSC_VER
#pragma GCC diagnostic pop
#else
//enable additional pragmas on MSVC
//#pragma warning(error:4062)	//incomplete switch
#pragma warning(error:4063) // case 'identifier' is not a valid value for switch of enum 'identifier'
#pragma warning(error:4091) // 'keyword' : ignored on left of 'type' when no variable is declared
#pragma warning(error:4265)	//'class': class has virtual functions, but destructor is not virtual
#pragma warning(error:4296)	//expression is always false
#pragma warning(error:4701)	//use of uninitialized variable
#pragma warning(error:4458) //declaration of 'x' hides class member
#pragma warning(error:4457) //declaration of 'x' hides function parameter
#pragma warning(error:4456) //declaration of 'y' hides previous local declaration
//#pragma warning(3:4702) //unreachable code
#endif
