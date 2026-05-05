#include "MyGameMode.h"
#include "../Player/MyPlayerCharacter.h"

AMyGameMode::AMyGameMode()
{
    DefaultPawnClass = AMyPlayerCharacter::StaticClass();
}