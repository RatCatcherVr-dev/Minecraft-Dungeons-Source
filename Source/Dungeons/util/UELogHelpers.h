#pragma once

#if NO_LOGGING
#define UE_LOG_OUTPUT(output, category, verbosity, fmt, ...)
#else
#define UE_LOG_OUTPUT(output, category, verbosity, fmt, ...) \
	do { \
		if (output) { \
			output->CategorizedLogf(category.GetCategoryName(), verbosity, fmt, __VA_ARGS__); \
		} else { \
			switch (verbosity) { \
			case ELogVerbosity::Fatal:       UE_LOG(category, Fatal, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::Error:       UE_LOG(category, Error, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::Warning:     UE_LOG(category, Warning, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::Display:     UE_LOG(category, Display, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::Log:         UE_LOG(category, Log, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::Verbose:     UE_LOG(category, Verbose, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::VeryVerbose: UE_LOG(category, VeryVerbose, fmt, __VA_ARGS__); break; \
			case ELogVerbosity::SetColor:    UE_LOG(category, SetColor, fmt, __VA_ARGS__); break; \
			default: \
				ensure(!"Invalid UE_LOG verbosity"); \
				break; \
			} \
		} \
	} while (0)
#endif

#ifndef LL_LOGGING
	#define LL_LOGGING !NO_LOGGING
#endif

#if LL_LOGGING
#define LL_LOGRAW(category, verbosity, fmt, ...) \
	do { \
		static_assert(TIsArrayOrRefOfType<decltype(fmt), TCHAR>::Value, "Formatting string must be a TCHAR array."); \
		static_assert((verbosity & ELogVerbosity::VerbosityMask) < ELogVerbosity::NumVerbosity && verbosity > 0, "Verbosity must be constant and in range."); \
		CA_CONSTANT_IF((verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && (ELogVerbosity::Warning & ELogVerbosity::VerbosityMask) <= FLogCategory##category::CompileTimeVerbosity) { \
			if (!category.IsSuppressed(verbosity)) { \
				FPlatformMisc::LowLevelOutputDebugStringf(fmt, __VA_ARGS__); \
			} \
		} \
	} while(0)
#else
#define LL_LOGRAW(category, verbosity, fmt, ...)
#endif


class FOutputDevice;

FOutputDevice& GetLogOutputOrDefault(FOutputDevice*);

using LogEntryPredicate = std::function<bool(const TCHAR*, ELogVerbosity::Type, const FName& category)>;


class LogRecorder : public FOutputDevice {
public:
	struct Entry {
		FString text;
		ELogVerbosity::Type verbosity;
		FName category;
	};
	void Serialize(const TCHAR*, ELogVerbosity::Type, const FName& category) override;

	const TArray<Entry>& getEntries() const;
private:
	TArray<Entry> mEntries;
};

void playbackLog(const TArray<LogRecorder::Entry>&, FOutputDevice&);


class ConditionalLogAdapter : public FOutputDevice {
public:
	ConditionalLogAdapter(FOutputDevice&, LogEntryPredicate);

	void Serialize(const TCHAR*, ELogVerbosity::Type, const FName& category) override;
private:
	FOutputDevice& mOutput;
	LogEntryPredicate mPredicate;
};
