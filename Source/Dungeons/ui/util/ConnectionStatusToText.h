#pragma once 

enum class EMinecraftAPIConnectionStatus : uint8;

namespace connection { namespace status {

FText ConnectionStatusToTextLabel(EMinecraftAPIConnectionStatus status);

}}
