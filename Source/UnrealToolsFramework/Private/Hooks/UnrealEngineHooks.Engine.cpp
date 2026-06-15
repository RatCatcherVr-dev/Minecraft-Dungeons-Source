//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHooks.Engine.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

// UTF & TDK RPC Includes
#include "RpcErrorCodes.h"
#include "UnrealEngineHooks.h"

// Unreal Engine Includes
#include "Engine/Console.h"

namespace Microsoft { namespace Internal { namespace GamesTest { namespace UnrealToolsFramework {

using namespace GamesTest::Rpc::Server;

RpcHookCode GetCurrentMapData(RpcArchive& Input, RpcArchive& Output)
{
    UNREFERENCED_PARAMETER(Input);

    UWorld* World = GetWorld();
    if (World)
    {
        HRESULT Result = S_OK;

        HRCHK(Output.SetNamedStringValue(TEXT("MapName"), *(World->GetMapName())))
        HRCHK(Output.SetNamedStringValue(TEXT("MapUrl"), *(World->URL.ToString())))
        
        return Result;
    }

    RETURN_ELEMNOTFND("World");
}


RpcHookCode LoadMap(RpcArchive& Input, RpcArchive& Output)
{
    UWorld* World = GetWorld();
    if (World)
    {
        HRESULT Result = S_OK;

        FString Url;
        HRCHK(GetStringValue(TEXT("Url"), Input, &Url))

        UGameplayStatics::OpenLevel(World, *Url);
        return Result;
    }

    UNREFERENCED_PARAMETER(Output);
    RETURN_ELEMNOTFND("World");
}


RpcHookCode GetEnumValues(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;
    HRESULT ElementsResult = S_OK;

    // Get the enum name.
    FString EnumName;
    HRCHK(GetStringValue(TEXT("Enum"), Input, &EnumName))

    const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true);
    if (EnumPtr)
    {
        // Get all the enum values (called names in UE).
        TArray<RpcArchive> EnumNames;
        int32 Count = EnumPtr->NumEnums();
        for (int32 Idx = 0; Idx < Count; ++Idx)
        {
            RpcArchive NameArchive;

            FString Name = EnumPtr->GetNameStringByIndex(Idx);
            if (*Name)
            {
                HRCHKELEM(NameArchive.SetNamedStringValue(TEXT("Name"), *Name))
            }
            else
            {
                ElementsResult = UTF_E_NOT_ALL_ELEMENTS_INCLUDED;
                continue;
            }

            const FString DisplayName = EnumPtr->GetDisplayNameTextByIndex(Idx).ToString();
            if (*DisplayName)
            {
                NameArchive.SetNamedStringValue(TEXT("DisplayName"), *DisplayName);
            }

            EnumNames.Add(NameArchive);
        }

        HRCHK(Output.SetNamedCollection(TEXT("EnumNames"), EnumNames.GetData(), EnumNames.Num()))

        // If there was a problem before this point HRCHK would have caused a return of
        // 'Result', so returning 'ElementsResult' instead to show this valuable warning.
        return ElementsResult;
    }

    RETURN_ELEMNOTFND("Enum")
}


RpcHookCode ExecuteCommand(RpcArchive& Input, RpcArchive& Output)
{
    HRESULT Result = S_OK;

    // Get command to execute.
    FString Command;
    HRCHK(GetStringValue(TEXT("Command"), Input, &Command))

    // Get game's console.    
    UConsole* Console = GetGameConsole();
	if (Console)
	{
		// If a target player was specified for the next console command, execute the command in that player's context.
		if (Console->ConsoleTargetPlayer && Console->ConsoleTargetPlayer->PlayerController &&
			Console->ConsoleTargetPlayer->PlayerController->Player)
		{
			FString CmdOutput;
			HRCHK(PlayerConsoleCommand(Command, Console->ConsoleTargetPlayer->PlayerController, CmdOutput));
			return Output.SetNamedStringValue(TEXT("CmdOutput"), *CmdOutput);
		}

		// Otherwise, execute the command in the context of the viewport.
		UGameViewportClient* ViewportClient = Console->GetOuterUGameViewportClient();
		if (ViewportClient)
		{
			FString CmdOutput;
			HRCHK(ViewportConsoleCommand(Command, ViewportClient, CmdOutput));
				return Output.SetNamedStringValue(TEXT("CmdOutput"), *CmdOutput);
		}
	}

    // If there are any players, execute the command in the first player's context which has a non-null player.
    UWorld* World = GetWorld();
    if (World && World->GetPlayerControllerIterator())
	{
        for (auto Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
            APlayerController* PlayerController = Iterator->Get();
            if (PlayerController && PlayerController->Player)
            {
                FString CmdOutput;
                HRCHK(PlayerConsoleCommand(Command, PlayerController, CmdOutput))
                return Output.SetNamedStringValue(TEXT("CmdOutput"), *CmdOutput);
            }
		}
	}

    RETURN_ELEMNOTFND("ContextToExecuteCommand");
}


RpcHookCode GetAutoCompleteCommands(RpcArchive& Input, RpcArchive& Output)
{
	HRESULT Result = S_OK;

	UConsole* console = (GEngine->GameViewport != nullptr) ? GEngine->GameViewport->ViewportConsole : nullptr;

	if (console == nullptr)
	{
		RETURN_ELEMNOTFND("Console");
	}

	if (!console->bIsRuntimeAutoCompleteUpToDate)
	{
		console->BuildRuntimeAutoCompleteList(true);
	}

	TArray<RpcArchive> commands;
	commands.Reserve(console->AutoCompleteList.Num());

	TIndexedContainerIterator<const TArray<FAutoCompleteCommand>, const FAutoCompleteCommand, int32> iter = console->AutoCompleteList.CreateConstIterator();

	while (iter)
	{
		RpcArchive command;
		HRCHK(command.SetNamedStringValue(TEXT("Command"), *(iter->Command)));
		HRCHK(command.SetNamedStringValue(TEXT("Description"), *(iter->Desc)));
		commands.Add(command);
		iter++;
	}

	return Output.SetNamedCollection(TEXT("AutoCompleteCommands"), commands.GetData(), commands.Num());
}

}}}}

#endif // ENABLE_GAMESTEST_RPC
