#pragma once

class APlayerCharacter;
class APlayerControllerBase;
class UWorld;

namespace playerquery {

APlayerCharacter* getPrimaryPlayerCharacter(UWorld*);
APlayerCharacter* getFirstLocalPlayerCharacter(UWorld*);
APlayerControllerBase* getFirstLocalPlayerController(UWorld*);

}
