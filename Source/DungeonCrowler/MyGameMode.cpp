#include "MyGameMode.h"
#include "MyPlayerCharacter.h"
#include "CrawGS.h"

AMyGameMode::AMyGameMode()
{
    DefaultPawnClass = AMyPlayerCharacter::StaticClass();
    GameStateClass = ACrawGS::StaticClass();
}