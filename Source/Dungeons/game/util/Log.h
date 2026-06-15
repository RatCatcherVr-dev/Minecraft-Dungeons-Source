#pragma once

#include "Dungeons.h"
#include <Logging/LogMacros.h>
#include <sstream>


namespace detail {

	class Logger
	{
	public:
		Logger(void) {
			mBuffer.clear();
			mBuffer << TEXT("[frame:") << GFrameCounter
				<< TEXT("|threadid:") << std::this_thread::get_id()
				<< TEXT("]: ");
		}

		~Logger(void) {
			UE_LOG(LogDungeons, Log, TEXT("%s"), mBuffer.str().c_str());
		}

		template <typename T>
		inline Logger& operator<<(T&& arg) { mBuffer << std::forward<T>(arg); return *this; }

	private:
		std::wstringstream mBuffer;
	};


	class LoggingDisabled
	{
	public:
		template <typename T>
		inline LoggingDisabled& operator<<(T&& arg) { return *this; }
	};
}

#ifdef UE_BUILD_DEBUG
using Log = detail::Logger;
#else
using Log = detail::LoggingDisabled;
#endif