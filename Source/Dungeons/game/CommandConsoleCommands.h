#pragma once

FString GenerateRunCommandsString(const TArray<FString>&);

FAutoConsoleCommand MakeRedirectCommand(const FString& cmd, const FString& help, const FString& redirectedToCmd, EConsoleVariableFlags, bool appendCmdToHelp = true);
