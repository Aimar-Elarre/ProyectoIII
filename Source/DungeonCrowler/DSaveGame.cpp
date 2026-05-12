#include "DSaveGame.h"

UDSaveGame::UDSaveGame()
	: SaveDateTime(FDateTime::Now()),
	SaveSlotName(TEXT("AutoCheckpointSlot")),
	UserIndex(0)
{
}
