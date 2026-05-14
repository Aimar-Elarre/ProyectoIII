#include "MyGameMode.h"
#include "DungeonGameState.h"
#include "../Player/MyPlayerCharacter.h"

AMyGameMode::AMyGameMode()
{
    DefaultPawnClass = AMyPlayerCharacter::StaticClass();
    GameStateClass = ADungeonGameState::StaticClass();
}