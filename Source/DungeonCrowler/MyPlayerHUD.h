#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MyPlayerHUD.generated.h"

UCLASS()
class DUNGEONCROWLER_API UMyPlayerHUD : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UProgressBar* StaminaBar;

    void UpdateStamina(float Percent);
};