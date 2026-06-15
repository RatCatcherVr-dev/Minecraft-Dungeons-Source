#include "Dungeons.h"
#include "UELogHelpers.h"
#include <OutputDeviceNull.h>
#include <CoreGlobals.h>

FOutputDevice& GetLogOutputOrDefault(FOutputDevice* output) {
	if (output) {
		return *output;
	}
	static FOutputDeviceNull nullLogger;
	return NO_LOGGING ? static_cast<FOutputDevice&>(nullLogger) : *GLog;
}


//
// LogRecorder + helpers
//
void LogRecorder::Serialize(const TCHAR* text, ELogVerbosity::Type verbosity, const FName& category) {
	mEntries.Add({ text, verbosity, category });
}

const TArray<LogRecorder::Entry>& LogRecorder::getEntries() const {
	return mEntries;
}

void playbackLog(const TArray<LogRecorder::Entry>& entries, FOutputDevice& output) {
	for (auto& entry : entries) {
		output.Serialize(*entry.text, entry.verbosity, entry.category);
	}
}


//
// ConditionalLogAdapter
//
ConditionalLogAdapter::ConditionalLogAdapter(FOutputDevice& output, LogEntryPredicate predicate)
	: mOutput(output)
	, mPredicate(predicate) {
}

void ConditionalLogAdapter::Serialize(const TCHAR* text, ELogVerbosity::Type verbosity, const FName& category) {
	if (mPredicate(text, verbosity, category)) {
		mOutput.Serialize(text, verbosity, category);
	}
}
