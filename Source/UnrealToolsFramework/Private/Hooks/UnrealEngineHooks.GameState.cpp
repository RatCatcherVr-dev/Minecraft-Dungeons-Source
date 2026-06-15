//------------------------------------------------------------------------------
// <copyright file="UnrealEngineHooks.GameState.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"

#ifdef ENABLE_GAMESTEST_RPC

#include <vector>

#include "UnrealEngineHooks.h"
#include "RpcErrorCodes.h"
#include "GameStateQuery.h"

namespace Microsoft {
	namespace Internal {
		namespace GamesTest {
			namespace UnrealToolsFramework {

				using namespace GamesTest::Rpc::Server;

				/// <summary>
				/// Extracts the filter criteria from an input RpcArchive.
				/// </summary>
				/// <typeparam name="AllocatorType">The query array allocator.</typeparam>
				/// <param name="Input">The input RpcArchive.</param>
				/// <param name="Query">The query filter.</param>
				/// <returns>Result code of the function.</returns>
				template<typename AllocatorType>
				static RpcHookCode ExtractFilters(const RpcArchive& Input, GameStateQuery<AllocatorType>& Query)
				{
					HRESULT Result = S_OK;

					FString ClassName;
					if (Input.HasNamedValue(TEXT("ClassName")))
					{
						HRCHK(GetStringValue(TEXT("ClassName"), Input, &ClassName));
						if (ClassName.Len() > 0)
						{
							Query.SetClass(*ClassName);
							// If the class name isn't valid, this will be null and match no objects.

							if (Query.GetClass() == nullptr) {
								return UTF_E_A_REQUIRED_ELEMENT_WAS_NOT_FOUND;
							}
						}
					}

					if (Input.HasNamedValue(TEXT("ObjectName")))
					{
						FString Name;
						HRCHK(GetStringValue(TEXT("ObjectName"), Input, &Name));
						Query.AddSearchString(Name);
					}
					else if (Input.HasNamedValue(TEXT("ObjectNames")))
					{
						TArray<FString, AllocatorType> Tmp;
						HRCHK(GetStringCollection(TEXT("ObjectNames"), Input, Tmp));
						for (const FString& Str : Tmp)
						{
							Query.AddSearchString(Str);
						}
					}

					return S_OK;
				}

				// Code to extract UPROPERTY to an array taken from Engine\Source\Runtime\CoreUObject\Private\UObject\Obj.cpp
				static RpcHookCode ExtractObjectProperty_Array(UObject* CurrentObject, RpcArchive& Output, const FString& FullPropertyName, UProperty* Property)
				{
					uint8* BaseData = Property->ContainerPtrToValuePtr<uint8>(CurrentObject);
					int32 ElementCount = Property->ArrayDim;

					UProperty* ExportProperty = Property;
					if (Property->ArrayDim == 1)
					{
						UArrayProperty* ArrayProp = dynamic_cast<UArrayProperty*>(Property);
						FScriptArrayHelper ArrayHelper(ArrayProp, BaseData);

						BaseData = ArrayHelper.GetRawPtr();
						ElementCount = ArrayHelper.Num();
						ExportProperty = ArrayProp->Inner;
					}

					int32 ElementSize = ExportProperty->ElementSize;
					TArray<FString> Elements;
					Elements.Reserve(ElementCount);
					for (int32 ArrayIndex = 0; ArrayIndex < ElementCount; ArrayIndex++)
					{
						FString ResultStr;
						uint8* ElementData = BaseData + ArrayIndex * ElementSize;
						ExportProperty->ExportTextItem(ResultStr, ElementData, NULL, CurrentObject, PPF_IncludeTransient);
						Elements.Add(ResultStr);
					}

					return SetStringCollection(*FullPropertyName, Output, Elements);
				}

				// Code to extract UPROPERTY to a string taken from Engine\Source\Runtime\CoreUObject\Private\UObject\Obj.cpp
				static RpcHookCode ExtractObjectProperty_String(UObject* CurrentObject, RpcArchive& Output, const FString& FullPropertyName, UProperty* Property)
				{
					uint8* BaseData = (uint8*)CurrentObject;

					FString ResultStr;
					for (int32 i = 0; i < Property->ArrayDim; i++)
					{
						Property->ExportText_InContainer(i, ResultStr, BaseData, BaseData, CurrentObject, PPF_IncludeTransient);
					}

					return Output.SetNamedStringValue(*FullPropertyName, *ResultStr);
				}

				static RpcHookCode ExtractObjectInfo(const UObject* CurrentObject, RpcArchive& Output)
				{
					HRESULT Result = S_OK;
					const UClass* CurrentObjectClass = CurrentObject->GetClass();
					HRCHK(Output.SetNamedStringValue(TEXT("class"), *CurrentObjectClass->GetName()));
					HRCHK(Output.SetNamedStringValue(TEXT("name"), *CurrentObject->GetPathName()));
					return Result;
				}

				// If the object property doesn't exist, check if it's a custom property for actors.
				static RpcHookCode ExtractActorProperty(const AActor* CurrentActor, RpcArchive& Output, const FString& FullPropertyName, const FString& PropertyName)
				{
					if (PropertyName == "location")
					{
						FVector Location = CurrentActor->GetActorLocation();
						return Output.SetNamedStringValue(*FullPropertyName, *Location.ToString());
					}
					else if (PropertyName == "rotation")
					{
						FRotator Rotation = CurrentActor->GetActorRotation();
						return Output.SetNamedStringValue(*FullPropertyName, *Rotation.ToString());
					}

					return S_OK;
				}

				// Recursive helper which descends into the object if a sub-property is queried.
				static RpcHookCode ExtractObjectProperty(UObject* CurrentObject, RpcArchive& Output, const FString& FullPropertyName, const FString& PropertyName)
				{
					const UClass* CurrentObjectClass = CurrentObject->GetClass();

					FString ComponentString, SubpropertyString;
					if (PropertyName.Split(TEXT("."), &ComponentString, &SubpropertyString))
					{
						UObjectPropertyBase* ComponentProperty = FindField<UObjectPropertyBase>(CurrentObjectClass, *ComponentString);
						if (ComponentProperty != nullptr)
						{
							CurrentObject = Cast<UObject>(ComponentProperty->GetObjectPropertyValue_InContainer(CurrentObject));
							if (CurrentObject != nullptr)
							{
								return ExtractObjectProperty(CurrentObject, Output, FullPropertyName, SubpropertyString);
							}
						}
					}
					else if (UProperty* Property = FindField<UProperty>(CurrentObjectClass, *PropertyName))
					{
						if (Property->ArrayDim > 1 || dynamic_cast<UArrayProperty*>(Property) != NULL)
						{
							return ExtractObjectProperty_Array(CurrentObject, Output, FullPropertyName, Property);
						}
						else
						{
							return ExtractObjectProperty_String(CurrentObject, Output, FullPropertyName, Property);
						}
					}
					else if (const AActor* CurrentActor = Cast<AActor>(CurrentObject))
					{
						return ExtractActorProperty(CurrentActor, Output, FullPropertyName, PropertyName);
					}

					return S_OK;
				}

				static RpcHookCode ExtractObjectProperties(UObject* QueryObject, RpcArchive& Output, const TArray<FString>& PropertyNames)
				{
					HRESULT Result = S_OK;
					HRCHK(ExtractObjectInfo(QueryObject, Output));

					for (const FString& PropertyName : PropertyNames)
					{
						HRCHK(ExtractObjectProperty(QueryObject, Output, PropertyName, PropertyName));
					}

					return Result;
				}

				RpcHookCode GetObjectProperties(RpcArchive& Input, RpcArchive& Output)
				{
					HRESULT Result = S_OK;

					UWorld* World = GetWorld();
					if (!World)
					{
						return UTF_E_WORLD_WAS_NOT_FOUND;
					}

					TArray<FString> PropertyNames;
					HRCHK(GetStringCollection(TEXT("Properties"), Input, PropertyNames));

					GameStateQuery<TInlineAllocator<1>> Query;
					HRCHK(ExtractFilters(Input, Query));

					TArray<UObject*, TInlineAllocator<16>> Matches;
					Query.GetMatchingObjects(World, Matches);

					std::vector<RpcArchive> Objects(Matches.Num());
					for (UObject* It : Matches)
					{
						RpcArchive Object;
						HRCHK(ExtractObjectProperties(It, Object, PropertyNames));
						Objects.push_back(Object);
					}

					HRCHK(Output.SetNamedCollection(TEXT("Objects"), Objects.data(), Objects.size()));
					return Result;
				}

				RpcHookCode GetActorProperties(RpcArchive& Input, RpcArchive& Output)
				{
					HRESULT Result = S_OK;

					UWorld* World = GetWorld();
					if (!World)
					{
						return UTF_E_WORLD_WAS_NOT_FOUND;
					}

					TArray<FString> PropertyNames;
					HRCHK(GetStringCollection(TEXT("Properties"), Input, PropertyNames));

					GameStateQuery<TInlineAllocator<1>> Query;
					HRCHK(ExtractFilters(Input, Query));

					TArray<AActor*, TInlineAllocator<16>> Matches;
					Query.GetMatchingActors(World, Matches);

					std::vector<RpcArchive> Actors(Matches.Num());
					for (AActor* It : Matches)
					{
						RpcArchive Actor;
						HRCHK(ExtractObjectProperties(It, Actor, PropertyNames));
						Actors.push_back(Actor);
					}

					HRCHK(Output.SetNamedCollection(TEXT("Actors"), Actors.data(), Actors.size()));
					return Result;
				}

			}
		}
	}
}

#endif // ENABLE_GAMESTEST_RPC
