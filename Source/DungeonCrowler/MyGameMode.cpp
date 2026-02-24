#include "MyGameMode.h"
#include "MyPlayerCharacter.h"

AMyGameMode::AMyGameMode()
{
    DefaultPawnClass = AMyPlayerCharacter::StaticClass();
}