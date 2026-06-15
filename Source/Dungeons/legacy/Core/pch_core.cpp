// stdafx.cpp : source file that includes just the standard includes
// Core.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "pch_core.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

void DEFAULT_ASSERT_HANDLER(const char* desc, const char* arg, const char* info, int line, const char* file, const char* function) {
// Calling the "legacy" asserts will no longer do anything, but they didn't work previously either.
}
AssertHandlerPtr gp_assert_handler = DEFAULT_ASSERT_HANDLER;