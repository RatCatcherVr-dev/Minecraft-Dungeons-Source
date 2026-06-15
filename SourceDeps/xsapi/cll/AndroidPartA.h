// Copyright (c) Microsoft. All rights reserved.

// Populates Android specific Part A fields

#pragma once

#include <jni.h>
#include <string>

#include "CllPartA.h"

namespace cll
{
    class AndroidPartA : public cll::CllPartA
    {
    public:
        AndroidPartA(JNIEnv * env, jobject context, const std::string & iKey);

    protected:
        std::string collectOsVer(JNIEnv * env);
        std::string collectOsLocale(JNIEnv * env);
        std::string collectAppId(JNIEnv * env, jobject context);
        std::string collectAppVer(JNIEnv * env, jobject context);
        std::string collectDeviceId(JNIEnv * env, jobject context);
        std::string collectDeviceClass(JNIEnv * env, jobject context);
        std::string collectUserId(JNIEnv * env, jobject context);

        // helper methods
        std::string getAndroidId(JNIEnv * env, jobject context);
        std::string getMacAddress(JNIEnv * env, jobject context);
        double getScreenSizeSquaredInInches(JNIEnv * env, jobject context);
        jstring getPackageName(JNIEnv * env, jobject context);
        std::string getProcessName(JNIEnv * env);
        jstring getRadioVersion(JNIEnv * env);
        jstring getAccountEmail(JNIEnv * env, jobject context);
        std::string hashWithSalt(JNIEnv * env, jstring inputString, jstring salt);
        std::string getUtf8StringFromJstring(JNIEnv * env, jstring inputString);
        jstring getJstringFromUtf8String(JNIEnv * env, const std::string & inputString);
    };
}
