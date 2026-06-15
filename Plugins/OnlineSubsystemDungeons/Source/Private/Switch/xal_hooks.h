#pragma once

#include <httpClient/async.h>

HRESULT InitXalBasicHooks();
HRESULT InitXalHooks(XTaskQueueHandle queue, char const* userSavegameRoot);
