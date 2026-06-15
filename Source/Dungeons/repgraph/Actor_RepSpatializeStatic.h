#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "game/actor/PropActor.h"
#include "Actor_RepSpatializeStatic.generated.h"

UCLASS()
class DUNGEONS_API AActor_RepSpatializeStatic : public AActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API APropActor_RepSpatializeStatic : public APropActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API AActor_RepSpatializeDynamic : public AActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API APropActor_RepSpatializeDynamic : public APropActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API AActor_RepSpatializeDormancy : public AActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API APropActor_RepSpatializeDormancy : public APropActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API AActor_RepAlways : public AActor
{
public:
	GENERATED_BODY()

	AActor_RepAlways();
};

UCLASS()
class DUNGEONS_API APropActor_RepAlways : public APropActor
{
public:
	GENERATED_BODY()

	APropActor_RepAlways();
};

UCLASS()
class DUNGEONS_API AActor_RepNotRouted : public AActor
{
public:
	GENERATED_BODY()
};

UCLASS()
class DUNGEONS_API APropActor_RepNotRouted : public APropActor
{
public:
	GENERATED_BODY()
};