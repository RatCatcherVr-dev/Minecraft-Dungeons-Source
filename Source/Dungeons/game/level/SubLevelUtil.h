#pragma once

class UWorld;
class FString;
class ULevelStreamingDynamic;
struct FTransform;

namespace game { namespace sublevel {

ULevelStreamingDynamic* createUnloaded(UWorld&, const FString& path, const FString& uniqueName);
ULevelStreamingDynamic* load(UWorld&, const FString& path, const FString& uniqueName, const FTransform& = {});

}}
