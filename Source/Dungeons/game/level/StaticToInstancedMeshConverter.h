#pragma once

class ULevelStreaming;

class AStaticToInstancedMeshConverter {
public:
	void Convert(ULevelStreaming*);
	void ConvertTo(ULevelStreaming*, bool, TArray<AActor*>&);
	void ConvertFrom(ULevelStreaming*, TArray<AActor*>&);
};
