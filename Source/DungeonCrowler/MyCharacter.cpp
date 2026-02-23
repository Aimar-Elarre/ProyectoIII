#include "MyCharacter.h"

AMyCharacter::AMyCharacter()
{
    Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}