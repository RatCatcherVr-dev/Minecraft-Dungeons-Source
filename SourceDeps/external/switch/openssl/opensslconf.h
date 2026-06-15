/* opensslconf.h */
/* WARNING: Generated automatically from opensslconf.h.in by Configure. */

#include <openssl/opensslv.h>

#ifdef  __cplusplus
extern "C" {
#endif
	/* OpenSSL was configured with the following options: */

#define NON_EMPTY_TRANSLATION_UNIT static void *dummy = &dummy;

#ifndef OPENSSL_DOING_MAKEDEPEND

#define OPENSSL_USE_IPV6 0

#define OPENSSL_NO_CAMELLIA 1

#ifndef OPENSSL_NO_DSO
# define OPENSSL_NO_SHA0
#endif
#ifndef OPENSSL_NO_DSO
# define OPENSSL_NO_DSO
#endif
#ifndef DSO_NONE
#define DSO_NONE
#endif
#ifndef OPENSSL_USE_NODELETE
# define OPENSSL_USE_NODELETE
#endif
#ifndef OPENSSL_NO_UI
# define OPENSSL_NO_UI
#endif
#ifndef OPENSSL_NO_ARIA
# define OPENSSL_NO_ARIA
#endif
#ifndef OPENSSL_NO_MD2
# define OPENSSL_NO_MD2
#endif
#ifndef OPENSSL_NO_RC5
# define OPENSSL_NO_RC5
#endif
//#ifndef OPENSSL_THREADS
//# define OPENSSL_THREADS
//#endif
#ifndef OPENSSL_NO_ASAN
# define OPENSSL_NO_ASAN
#endif
#ifndef OPENSSL_NO_ASM
# define OPENSSL_NO_ASM
#endif
#ifndef OPENSSL_NO_IDEA
#define OPENSSL_NO_IDEA
#endif
#ifndef OPENSSL_NO_CRYPTO_MDEBUG
# define OPENSSL_NO_CRYPTO_MDEBUG
#endif
#ifndef OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
# define OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
#endif
#ifndef OPENSSL_NO_EC_NISTP_64_GCC_128
# define OPENSSL_NO_EC_NISTP_64_GCC_128
#endif
#ifndef OPENSSL_NO_BF
# define OPENSSL_NO_BF
#endif
#ifndef OPENSSL_NO_CAST
# define OPENSSL_NO_CAST
#endif
#ifndef OPENSSL_NO_EGD
# define OPENSSL_NO_EGD
#endif
#ifndef OPENSSL_NO_EXTERNAL_TESTS
# define OPENSSL_NO_EXTERNAL_TESTS
#endif
#ifndef OPENSSL_NO_FUZZ_AFL
# define OPENSSL_NO_FUZZ_AFL
#endif
#ifndef OPENSSL_NO_FUZZ_LIBFUZZER
# define OPENSSL_NO_FUZZ_LIBFUZZER
#endif
#ifndef OPENSSL_NO_HEARTBEATS
# define OPENSSL_NO_HEARTBEATS
#endif
#ifndef OPENSSL_NO_MSAN
# define OPENSSL_NO_MSAN
#endif
#ifndef OPENSSL_NO_SCTP
# define OPENSSL_NO_SCTP
#endif
#ifndef OPENSSL_NO_SSL_TRACE
# define OPENSSL_NO_SSL_TRACE
#endif
#ifndef OPENSSL_NO_TLS1_3
# define OPENSSL_NO_TLS1_3
#endif
#ifndef OPENSSL_NO_UBSAN
# define OPENSSL_NO_UBSAN
#endif
#ifndef OPENSSL_NO_UNIT_TEST
# define OPENSSL_NO_UNIT_TEST
#endif
#ifndef OPENSSL_NO_WEAK_SSL_CIPHERS
# define OPENSSL_NO_WEAK_SSL_CIPHERS
#endif
#ifndef OPENSSL_NO_AFALGENG
# define OPENSSL_NO_AFALGENG
#endif
#ifndef OPENSSL_NO_DYNAMIC_ENGINE
# define OPENSSL_NO_DYNAMIC_ENGINE
#endif

#ifndef OPENSSL_NO_HW_4758_CCA
# define OPENSSL_NO_HW_4758_CCA
#endif
#ifndef OPENSSL_NO_HW_AEP
# define OPENSSL_NO_HW_AEP
#endif
#ifndef OPENSSL_NO_HW_ATALLA
# define OPENSSL_NO_HW_ATALLA
#endif
#ifndef OPENSSL_NO_HW_CSWIFT
# define OPENSSL_NO_HW_CSWIFT
#endif
#ifndef OPENSSL_NO_HW_NCIPHER
# define OPENSSL_NO_HW_NCIPHER
#endif
#ifndef OPENSSL_NO_HW_NURON
# define OPENSSL_NO_HW_NURON
#endif
#ifndef OPENSSL_NO_HW_SUREWARE
# define OPENSSL_NO_HW_SUREWARE
#endif
#ifndef OPENSSL_NO_HW_UBSEC
# define OPENSSL_NO_HW_UBSEC
#endif
#ifndef OPENSSL_NO_GOST
# define OPENSSL_NO_GOST
#endif

#ifndef OPENSSL_NO_GMP
# define OPENSSL_NO_GMP
#endif
#ifndef OPENSSL_NO_JPAKE
# define OPENSSL_NO_JPAKE
#endif
#ifndef OPENSSL_NO_KRB5
# define OPENSSL_NO_KRB5
#endif
#ifndef OPENSSL_NO_MD4
# define OPENSSL_NO_MD4
#endif
#ifndef OPENSSL_NO_MDC2
# define OPENSSL_NO_MDC2
#endif
#ifndef OPENSSL_NO_RIPEMD
# define OPENSSL_NO_RIPEMD
#endif
#ifndef OPENSSL_NO_LIBUNBOUND
# define OPENSSL_NO_LIBUNBOUND
#endif
#ifndef OPENSSL_NO_RFC3779
# define OPENSSL_NO_RFC3779
#endif
#ifndef OPENSSL_NO_STORE
# define OPENSSL_NO_STORE
#endif

#ifndef OPENSSL_NO_BLAKE2
#define OPENSSL_NO_BLAKE2
#endif
#ifndef OPENSSL_NO_DEVCRYPTOENG
# define OPENSSL_NO_DEVCRYPTOENG
#endif
#ifndef OPENSSL_NO_UI_CONSOLE
# define OPENSSL_NO_UI_CONSOLE
#endif
#ifndef OPENSSL_NO_SECURE_MEMORY
# define OPENSSL_NO_SECURE_MEMORY
#endif
#endif /* OPENSSL_DOING_MAKEDEPEND */

#ifndef OPENSSL_NO_DYNAMIC_ENGINE
# define OPENSSL_NO_DYNAMIC_ENGINE
#endif

#ifndef DECLARE_DEPRECATED
# define DECLARE_DEPRECATED(f)   f;
# ifdef __GNUC__
#  if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0)
#   undef DECLARE_DEPRECATED
#   define DECLARE_DEPRECATED(f)    f __attribute__ ((deprecated));
#  endif
# endif
#endif

#ifndef OPENSSL_MIN_API
# define OPENSSL_MIN_API 0
#endif

#if !defined(OPENSSL_API_COMPAT) || OPENSSL_API_COMPAT < OPENSSL_MIN_API
# undef OPENSSL_API_COMPAT
# define OPENSSL_API_COMPAT OPENSSL_MIN_API
#endif

/*
 * Do not deprecate things to be deprecated in version 1.2.0 before the
 * OpenSSL version number matches.
 */
#if OPENSSL_VERSION_NUMBER < 0x10200000L
# define DEPRECATEDIN_1_2_0(f)   f;
#elif OPENSSL_API_COMPAT < 0x10200000L
# define DEPRECATEDIN_1_2_0(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_1_2_0(f)
#endif

#if OPENSSL_API_COMPAT < 0x10100000L
# define DEPRECATEDIN_1_1_0(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_1_1_0(f)
#endif

#if OPENSSL_API_COMPAT < 0x10000000L
# define DEPRECATEDIN_1_0_0(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_1_0_0(f)
#endif

#if OPENSSL_API_COMPAT < 0x00908000L
# define DEPRECATEDIN_0_9_8(f)   DECLARE_DEPRECATED(f)
#else
# define DEPRECATEDIN_0_9_8(f)
#endif

#ifndef OPENSSL_FILE
# ifdef OPENSSL_NO_FILENAMES
#  define OPENSSL_FILE ""
#  define OPENSSL_LINE 0
# else
#  define OPENSSL_FILE __FILE__
#  define OPENSSL_LINE __LINE__
# endif
#endif

 /* The OPENSSL_NO_* macros are also defined as NO_* if the application
	asks for it.  This is a transient feature that is provided for those
	who haven't had the time to do the appropriate changes in their
	applications.  */
#ifdef OPENSSL_ALGORITHM_DEFINES
# if defined(OPENSSL_NO_EC_NISTP_64_GCC_128) && !defined(NO_EC_NISTP_64_GCC_128)
#  define NO_EC_NISTP_64_GCC_128
# endif
# if defined(OPENSSL_NO_GMP) && !defined(NO_GMP)
#  define NO_GMP
# endif
# if defined(OPENSSL_NO_JPAKE) && !defined(NO_JPAKE)
#  define NO_JPAKE
# endif
# if defined(OPENSSL_NO_KRB5) && !defined(NO_KRB5)
#  define NO_KRB5
# endif
# if defined(OPENSSL_NO_LIBUNBOUND) && !defined(NO_LIBUNBOUND)
#  define NO_LIBUNBOUND
# endif
# if defined(OPENSSL_NO_MD2) && !defined(NO_MD2)
#  define NO_MD2
# endif
# if defined(OPENSSL_NO_RC5) && !defined(NO_RC5)
#  define NO_RC5
# endif
# if defined(OPENSSL_NO_RFC3779) && !defined(NO_RFC3779)
#  define NO_RFC3779
# endif
# if defined(OPENSSL_NO_SCTP) && !defined(NO_SCTP)
#  define NO_SCTP
# endif
# if defined(OPENSSL_NO_SSL_TRACE) && !defined(NO_SSL_TRACE)
#  define NO_SSL_TRACE
# endif
# if defined(OPENSSL_NO_STORE) && !defined(NO_STORE)
#  define NO_STORE
# endif
# if defined(OPENSSL_NO_UNIT_TEST) && !defined(NO_UNIT_TEST)
#  define NO_UNIT_TEST
# endif
#endif

	/* crypto/opensslconf.h.in */

	/* Generate 80386 code? */
#undef I386_ONLY

#if !(defined(VMS) || defined(__VMS)) && !defined(__ORBIS__) && !defined(NN_NINTENDO_SDK) /* VMS uses logical names instead */
#if defined(HEADER_CRYPTLIB_H) && !defined(OPENSSLDIR)
#define ENGINESDIR "/usr/local/ssl/lib/engines"
#define OPENSSLDIR "/usr/local/ssl"
#endif
#endif

#if defined(NN_NINTENDO_SDK)

#endif

#undef OPENSSL_UNISTD
#define OPENSSL_UNISTD <unistd.h>

#undef OPENSSL_EXPORT_VAR_AS_FUNCTION

//#if defined(HEADER_IDEA_H) && !defined(IDEA_INT)
//#define IDEA_INT unsigned int
//#endif

#if defined(HEADER_MD2_H) && !defined(MD2_INT)
#define MD2_INT unsigned int
#endif

//#if defined(HEADER_RC2_H) && !defined(RC2_INT)
///* I need to put in a mod for the alpha - eay */
//#define RC2_INT unsigned int
//#endif

#if defined(HEADER_RC4_H)
#if !defined(RC4_INT)
/* using int types make the structure larger but make the code faster
 * on most boxes I have tested - up to %20 faster. */
 /*
  * I don't know what does "most" mean, but declaring "int" is a must on:
  * - Intel P6 because partial register stalls are very expensive;
  * - elder Alpha because it lacks byte load/store instructions;
  */
#define RC4_INT unsigned int
#endif
#if !defined(RC4_CHUNK)
  /*
   * This enables code handling data aligned at natural CPU word
   * boundary. See crypto/rc4/rc4_enc.c for further details.
   */
#undef RC4_CHUNK
#endif
#endif

   //#if (defined(HEADER_NEW_DES_H) || defined(HEADER_DES_H)) && !defined(DES_LONG)
   ///* If this is set to 'unsigned int' on a DEC Alpha, this gives about a
   // * %20 speed up (longs are 8 bytes, int's are 4). */
   //#ifndef DES_LONG
   //#define DES_LONG unsigned long
   //#endif
   //#endif

#if defined(HEADER_BN_H) && !defined(CONFIG_HEADER_BN_H)
#define CONFIG_HEADER_BN_H
#undef BN_LLONG

/* Should we define BN_DIV2W here? */

/* Only one for the following should be defined */
#undef SIXTY_FOUR_BIT_LONG
#undef SIXTY_FOUR_BIT
#define THIRTY_TWO_BIT
#endif

#if defined(HEADER_RC4_LOCL_H) && !defined(CONFIG_HEADER_RC4_LOCL_H)
#define CONFIG_HEADER_RC4_LOCL_H
/* if this is defined data[i] is used instead of *data, this is a %20
 * speedup on x86 */
#undef RC4_INDEX
#endif

#if defined(HEADER_BF_LOCL_H) && !defined(CONFIG_HEADER_BF_LOCL_H)
#define CONFIG_HEADER_BF_LOCL_H
#undef BF_PTR
#endif /* HEADER_BF_LOCL_H */

#if defined(HEADER_DES_LOCL_H) && !defined(CONFIG_HEADER_DES_LOCL_H)
#define CONFIG_HEADER_DES_LOCL_H
#ifndef DES_DEFAULT_OPTIONS
 /* the following is tweaked from a config script, that is why it is a
  * protected undef/define */
#ifndef DES_PTR
#undef DES_PTR
#endif

  /* This helps C compiler generate the correct code for multiple functional
   * units.  It reduces register dependancies at the expense of 2 more
   * registers */
#ifndef DES_RISC1
#undef DES_RISC1
#endif

#ifndef DES_RISC2
#undef DES_RISC2
#endif

#if defined(DES_RISC1) && defined(DES_RISC2)
#error YOU SHOULD NOT HAVE BOTH DES_RISC1 AND DES_RISC2 DEFINED!!!!!
#endif

   /* Unroll the inner loop, this sometimes helps, sometimes hinders.
	* Very mucy CPU dependant */
#ifndef DES_UNROLL
#undef DES_UNROLL
#endif

	/* These default values were supplied by
	 * Peter Gutman <pgut001@cs.auckland.ac.nz>
	 * They are only used if nothing else has been defined */
#if !defined(DES_PTR) && !defined(DES_RISC1) && !defined(DES_RISC2) && !defined(DES_UNROLL)
	 /* Special defines which change the way the code is built depending on the
		CPU and OS.  For SGI machines you can use _MIPS_SZLONG (32 or 64) to find
		even newer MIPS CPU's, but at the moment one size fits all for
		optimization options.  Older Sparc's work better with only UNROLL, but
		there's no way to tell at compile time what it is you're running on */

#if defined( __sun ) || defined ( sun )		/* Newer Sparc's */
#  define DES_PTR
#  define DES_RISC1
#  define DES_UNROLL
#elif defined( __ultrix )	/* Older MIPS */
#  define DES_PTR
#  define DES_RISC2
#  define DES_UNROLL
#elif defined( __osf1__ )	/* Alpha */
#  define DES_PTR
#  define DES_RISC2
#elif defined ( _AIX )		/* RS6000 */
		/* Unknown */
#elif defined( __hpux )		/* HP-PA */
		/* Unknown */
#elif defined( __aux )		/* 68K */
		/* Unknown */
#elif defined( __dgux )		/* 88K (but P6 in latest boxes) */
#  define DES_UNROLL
#elif defined( __sgi )		/* Newer MIPS */
#  define DES_PTR
#  define DES_RISC2
#  define DES_UNROLL
#elif defined(i386) || defined(__i386__)	/* x86 boxes, should be gcc */
#  define DES_PTR
#  define DES_RISC1
#  define DES_UNROLL
#endif /* Systems-specific speed defines */
#endif

#endif /* DES_DEFAULT_OPTIONS */
#endif /* HEADER_DES_LOCL_H */
#ifdef  __cplusplus
}
#endif
