 // This is a Message Text file that is used to define custom error codes
 // and their associated message.  http://msdn.microsoft.com/en-us/library/windows/desktop/dd996906(v=vs.85).aspx
 // This file is used to generate the RpcErrorCodes.h file that you see #include'd in other places in this project.
 // This file is integrated into the Visual Studio build process using the same basic process as described here: http://stackoverflow.com/questions/3026855/configure-the-message-compiler-mc-exe-as-a-custom-compiler-step-in-vc-2010
 // I have learned the hard way that this file MUST end with a blank line, otherwise you will get an "invalid character" error at compile-time.
 /* --------------------------------------------------------
 HEADER SECTION
*/



/* ------------------------------------------------------------------
 MESSAGE DEFINITION SECTION
*/
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+---------------------+-------------------------------+
//  |S|R|C|N|r|    Facility         |               Code            |
//  +-+-+-+-+-+---------------------+-------------------------------+
//
//  where
//
//      S - Severity - indicates success/fail
//
//          0 - Success
//          1 - Fail (COERROR)
//
//      R - reserved portion of the facility code, corresponds to NT's
//              second severity bit.
//
//      C - reserved portion of the facility code, corresponds to NT's
//              C field.
//
//      N - reserved portion of the facility code. Used to indicate a
//              mapped NT status value.
//
//      r - reserved portion of the facility code. Reserved for internal
//              use. Used to indicate HRESULT values that are not status
//              values, but are instead message ids for display strings.
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_TDK_RPC                 0x1
#define FACILITY_SYSTEM                  0x0
#define FACILITY_STUBS                   0x3
#define FACILITY_RUNTIME                 0x2
#define FACILITY_IO_ERROR_CODE           0x4


//
// Define the severity codes
//
#define STATUS_SEVERITY_WARNING          0x2
#define STATUS_SEVERITY_SUCCESS          0x0
#define STATUS_SEVERITY_INFORMATIONAL    0x1
#define STATUS_SEVERITY_ERROR            0x3


//
// MessageId: GAMESTEST_RPC_E_METHOD_NOT_FOUND
//
// MessageText:
//
// Attempted to invoke an RPC method that does not exist
//
#define GAMESTEST_RPC_E_METHOD_NOT_FOUND ((HRESULT)0xE0010064L)

//
// MessageId: GAMESTEST_RPC_S_NOT_STARTED
//
// MessageText:
//
// The RpcServer was not previously started
//
#define GAMESTEST_RPC_S_NOT_STARTED      ((HRESULT)0x20010065L)

//
// MessageId: GAMESTEST_RPC_E_SERVER_ID_ALREADY_IN_USE
//
// MessageText:
//
// An RpcServer with that Id was already started
//
#define GAMESTEST_RPC_E_SERVER_ID_ALREADY_IN_USE ((HRESULT)0xE0010066L)

//
// MessageId: GAMESTEST_RPC_S_ALREADY_LISTENING
//
// MessageText:
//
// The RpcServer is already listening
//
#define GAMESTEST_RPC_S_ALREADY_LISTENING ((HRESULT)0x20010067L)

//
// MessageId: GAMESTEST_RPC_E_UNKNOWN_SERVER_ID
//
// MessageText:
//
// Unable to find a server with the given server Id
//
#define GAMESTEST_RPC_E_UNKNOWN_SERVER_ID ((HRESULT)0xE0010068L)

//
// MessageId: GAMESTEST_RPC_W_FAILED_TO_SERIALIZE_USER_DATA
//
// MessageText:
//
// Failed to serialize the user data portion of the response
//
#define GAMESTEST_RPC_W_FAILED_TO_SERIALIZE_USER_DATA ((HRESULT)0xA0010069L)

//
// MessageId: GAMESTEST_RPC_E_CLIENT_DISCONNECTED
//
// MessageText:
//
// The client has disconnected before a response could be sent
//
#define GAMESTEST_RPC_E_CLIENT_DISCONNECTED ((HRESULT)0xE001006AL)

//
// MessageId: GAMESTEST_RPC_E_FAILED_TO_PARSE_RPC_SOCKET_REQUEST
//
// MessageText:
//
// Failed to parse incoming socket-based RPC Request
//
#define GAMESTEST_RPC_E_FAILED_TO_PARSE_RPC_SOCKET_REQUEST ((HRESULT)0xE001006BL)

//
// MessageId: GAMESTEST_RPC_E_DESTINATION_BUFFER_TOO_SMALL
//
// MessageText:
//
// The buffer provided to the TDK RPC was too small to hold the required amount of data.
//
#define GAMESTEST_RPC_E_DESTINATION_BUFFER_TOO_SMALL ((HRESULT)0xE001006CL)

//
// MessageId: GAMESTEST_RPC_E_ARCHIVE_VALUE_IS_NULL
//
// MessageText:
//
// The non-string value contained in the RpcArchive is null.
//
#define GAMESTEST_RPC_E_ARCHIVE_VALUE_IS_NULL ((HRESULT)0xE001006DL)

//
// MessageId: GAMESTEST_RPC_E_MEMBER_NOT_FOUND_IN_ARCHIVE
//
// MessageText:
//
// The requested data member was not found in the RpcArchive.
//
#define GAMESTEST_RPC_E_MEMBER_NOT_FOUND_IN_ARCHIVE ((HRESULT)0xE001006EL)

//
// MessageId: GAMESTEST_RPC_E_PENDING_REQUESTS_QUEUE_FULL
//
// MessageText:
//
// The request was rejected because the server request queue is full.
//
#define GAMESTEST_RPC_E_PENDING_REQUESTS_QUEUE_FULL ((HRESULT)0xE001006FL)

//
// MessageId: GAMESTEST_RPC_I_PROCESS_PENDING_REQUEST_PARTIAL_FAILURE
//
// MessageText:
//
// There was an error processing one or more queued requests.
//
#define GAMESTEST_RPC_I_PROCESS_PENDING_REQUEST_PARTIAL_FAILURE ((HRESULT)0x60010070L)

//
// MessageId: GAMESTEST_RPC_E_PROCESS_PENDING_REQUEST_FAILURE
//
// MessageText:
//
// There was an error processing all queued requests.
//
#define GAMESTEST_RPC_E_PROCESS_PENDING_REQUEST_FAILURE ((HRESULT)0xE0010071L)

//
// MessageId: GAMESTEST_RPC_E_VERSION_MISMATCH
//
// MessageText:
//
// The version of the client does not match the version of the server.
//
#define GAMESTEST_RPC_E_VERSION_MISMATCH ((HRESULT)0xE0010072L)

//
// MessageId: GAMESTEST_RPC_E_METHOD_DISABLED_INVALID_SANDBOX
//
// MessageText:
//
// The method has been disabled due to being in an invalid sandbox.
//
#define GAMESTEST_RPC_E_METHOD_DISABLED_INVALID_SANDBOX ((HRESULT)0xE0010073L)

//
// MessageId: GAMESTEST_RPC_E_IMMEDIATE_METHOD_ALREADY_REGISTERED
//
// MessageText:
//
// An immediate method with this name has already been registered.
//
#define GAMESTEST_RPC_E_IMMEDIATE_METHOD_ALREADY_REGISTERED ((HRESULT)0xE0010074L)

