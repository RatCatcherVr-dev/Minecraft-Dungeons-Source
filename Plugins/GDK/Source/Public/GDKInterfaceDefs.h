#pragma once

typedef void __stdcall XGameInviteEventCallback(
	void* context,
	const char* inviteUri
);

struct GDKTaskQueueRegistrationToken
{
	uint64_t token;
};

typedef void* GDKTaskQueueHandle;
typedef void* XStoreContextHandle;

struct GDKAsyncBlock;

typedef void __stdcall GDKAsyncCompletionRoutine(
	GDKAsyncBlock* asyncBlock
);

typedef struct GDKAsyncBlock {
	GDKTaskQueueHandle queue;
	void* context;
	GDKAsyncCompletionRoutine* callback;
	unsigned internal[sizeof(void*) * 4];
} GDKAsyncBlock;
