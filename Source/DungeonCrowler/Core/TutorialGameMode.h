#pragma once

#include "CoreMinimal.h"
#include "MyGameMode.h"
#include "TutorialGameMode.generated.h"

/**
 * Tutorial GameMode: Features are unlocked via triggers (Inventory, Sprint, Dash)
 * Players must interact with unlock triggers to enable each feature.
 */
UCLASS()
class DUNGEONCROWLER_API ATutorialGameMode : public AMyGameMode
{
	GENERATED_BODY()

public:
	ATutorialGameMode();
};
