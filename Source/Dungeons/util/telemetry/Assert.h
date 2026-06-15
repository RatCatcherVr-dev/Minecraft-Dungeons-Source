#pragma once

namespace dungeons
{
	enum class AssertSeverityLevel
	{
		Error,
		Warning
	};

	void fensure_networked(bool condition
		, UObject* worldContextObject
		, const char* filename
		, int line
		, const char* functionName
		, AssertSeverityLevel severity = AssertSeverityLevel::Error
		, const char* message = nullptr);

} // end of namespace

#define ensure_networked(expr, context)	{ if (UNLIKELY(!(expr))) \
{																 \
	dungeons::fensure_networked(expr, context, __FILE__, __LINE__, __FUNCTION__, dungeons::AssertSeverityLevel::Error); \
	ensure(expr); \
} }

#define ensure_networked_msg(expr, context, msg) { if (UNLIKELY(!(expr))) \
{																		 \
	dungeons::fensure_networked(expr, context, __FILE__, __LINE__, __FUNCTION__, dungeons::AssertSeverityLevel::Error, msg); \
	ensureMsgf(expr, TEXT(msg)); \
} }
