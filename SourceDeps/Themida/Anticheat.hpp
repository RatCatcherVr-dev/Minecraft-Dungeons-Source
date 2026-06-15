// Anticheat Library for the ThemidaSDK
// by Elias Bachaalany
#pragma once

#ifdef USING_ANTICHEAT

#define ANTICHEAT_MACRO_PASTE(X, Y) X ## Y
#define ANTICHEAT_MACRO_EVAL(X, Y)  ANTICHEAT_MACRO_PASTE(X, Y)
#ifndef ANTICHEAT_CONFIG_USED_VM 
    #define ANTICHEAT_CONFIG_USED_VM VM_TIGER_RED
#endif

#include "ThemidaSDK.h"
#include <intrin.h>

// Use these 2 macros when protecting a block that includes the last 'return [expr]' in the function
// By disabling optimization, Themida functions better
#define ANTICHEAT_NO_OPTIMIZATION_BEGIN \
    __pragma(optimize("", off)) \
    __declspec(noinline)

#define ANTICHEAT_NO_OPTIMIZATION_END \
    __pragma(optimize("", on))

#define ANTICHEAT_PROTECT_STRINGS_BEGIN        STR_ENCRYPT_START; STR_ENCRYPTW_START;
#define ANTICHEAT_PROTECT_STRINGS_END          STR_ENCRYPTW_END; STR_ENCRYPT_END; __nop();

#define ANTICHEAT_PROTECT_STRING_BEGIN         STR_ENCRYPT_START
#define ANTICHEAT_PROTECT_STRING_END           STR_ENCRYPT_END; __nop();

#define ANTICHEAT_PROTECT_STRINGW_BEGIN        STR_ENCRYPTW_START
#define ANTICHEAT_PROTECT_STRINGW_END          STR_ENCRYPTW_END; __nop();

#define ANTICHEAT_STRINGS(x) __pragma(optimize("", off)) []() __declspec(noinline) { ANTICHEAT_PROTECT_STRINGS_BEGIN; auto v = x; ANTICHEAT_PROTECT_STRINGS_END; return v; }() __pragma(optimize("", on))

#define ANTICHEAT_OBFUSCATE_BEGIN              MUTATE_START
#define ANTICHEAT_OBFUSCATE_END                MUTATE_END; __nop();

#define ANTICHEAT_VIRT_BEGIN_WITH(vm)          vm ## _START
#define ANTICHEAT_VIRT_END_WITH(vm)            vm ## _END

#define ANTICHEAT_VIRT_BEGIN                   ANTICHEAT_MACRO_EVAL(ANTICHEAT_CONFIG_USED_VM, _START)
#define ANTICHEAT_VIRT_END                     ANTICHEAT_MACRO_EVAL(ANTICHEAT_CONFIG_USED_VM, _END); __nop();

#define ANTICHEAT_CHECK_USER_CODE_INTEGRITY(int_var, int_val) \
        CHECK_CODE_INTEGRITY(int_var, int_val); __nop();

#define ANTICHEAT_CHECK_PROTECTION_CODE_INTEGRITY(int_var, int_val) \
        CHECK_PROTECTION(int_var, int_val); __nop();

#define ANTICHEAT_CHECK_DEBUGGER(int_var, int_val) \
        CHECK_DEBUGGER(int_var, int_val); __nop();
#define ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN   ANTICHEAT_VIRT_BEGIN; ANTICHEAT_PROTECT_STRINGS_BEGIN;
#define ANTICHEAT_VIRT_PROTECT_STRINGS_END     ANTICHEAT_PROTECT_STRINGS_END; ANTICHEAT_VIRT_END; __nop();

#define ANTICHEAT_VIRT_PROTECT_STRING_BEGIN    ANTICHEAT_VIRT_BEGIN; ANTICHEAT_PROTECT_STRING_BEGIN;
#define ANTICHEAT_VIRT_PROTECT_STRING_END      ANTICHEAT_PROTECT_STRING_END; ANTICHEAT_VIRT_END; __nop();

#define ANTICHEAT_VIRT_PROTECT_STRINGW_BEGIN   ANTICHEAT_VIRT_BEGIN; ANTICHEAT_PROTECT_STRINGW_BEGIN;
#define ANTICHEAT_VIRT_PROTECT_STRINGW_END     ANTICHEAT_PROTECT_STRINGW_END; ANTICHEAT_VIRT_END; __nop();

#define ANTICHEAT_VIRT_CHECK_USER_CODE(int_var, int_val) \
		ANTICHEAT_VIRT_BEGIN; ANTICHEAT_CHECK_USER_CODE_INTEGRITY(int_var, int_val); ANTICHEAT_VIRT_END;

#define ANTICHEAT_PROTECTED_GLOBAL_STRING(x) \
        __pragma(optimize("", off)) []() __declspec(noinline) \
        { \
            ANTICHEAT_PROTECT_STRINGS_BEGIN; \
            auto v = x; \
            ANTICHEAT_PROTECT_STRINGS_END; \
            return v; \
        }() \
        __pragma(optimize("", on))
#else
#define ANTICHEAT_PROTECTED_GLOBAL_STRING(x) x

#define ANTICHEAT_NO_OPTIMIZATION_BEGIN
#define ANTICHEAT_NO_OPTIMIZATION_END

#define ANTICHEAT_PROTECT_STRINGS_BEGIN        
#define ANTICHEAT_PROTECT_STRINGS_END          

#define ANTICHEAT_PROTECT_STRING_BEGIN         
#define ANTICHEAT_PROTECT_STRING_END           

#define ANTICHEAT_PROTECT_STRINGW_BEGIN        
#define ANTICHEAT_PROTECT_STRINGW_END

#define ANTICHEAT_STRINGS(x) x

#define ANTICHEAT_OBFUSCATE_BEGIN              
#define ANTICHEAT_OBFUSCATE_END                

#define ANTICHEAT_VIRT_BEGIN_WITH(vm)          
#define ANTICHEAT_VIRT_END_WITH(vm)            

#define ANTICHEAT_VIRT_BEGIN                   
#define ANTICHEAT_VIRT_END                     

#define ANTICHEAT_CHECK_USER_CODE_INTEGRITY(int_var, int_val) \
        int_var = int_val;

#define ANTICHEAT_CHECK_PROTECTION_CODE_INTEGRITY(int_var, int_val) \
        int_var = int_val;

#define ANTICHEAT_CHECK_DEBUGGER(int_var, int_val) \
        int_var = int_val;
#define ANTICHEAT_VIRT_PROTECT_STRINGS_BEGIN
#define ANTICHEAT_VIRT_PROTECT_STRINGS_END

#define ANTICHEAT_VIRT_PROTECT_STRING_BEGIN
#define ANTICHEAT_VIRT_PROTECT_STRING_END

#define ANTICHEAT_VIRT_PROTECT_STRINGW_BEGIN
#define ANTICHEAT_VIRT_PROTECT_STRINGW_END

#define ANTICHEAT_VIRT_CHECK_USER_CODE(int_var, int_val) \
		int_var = int_val;

#endif
