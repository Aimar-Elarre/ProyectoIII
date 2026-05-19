#pragma once

#include "CoreMinimal.h"
#include "MyGameMode.h"
#include "NormalGameMode.generated.h"

/**
 * Normal GameMode: All features (Inventory, Sprint, Dash) are unlocked automatically at game start.
 * No trigger interactions required - player has full access to all mechanics from the beginning.
 */
UCLASS()
class DUNGEONCROWLER_API ANormalGameMode : public AMyGameMode
{
	GENERATED_BODY()

public:
	ANormalGameMode();
};
