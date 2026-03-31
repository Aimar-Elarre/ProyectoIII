#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MyPlayerHUD.generated.h"

UCLASS()
class DUNGEONCROWLER_API UMyPlayerHUD : public UUserWidget
{
    GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HintText;

    void ShowHint(const FString& Message);
    void HideHint();
    UPROPERTY(meta = (BindWidget))
    UProgressBar* StaminaBar;

    UPROPERTY(meta = (BindWidget))
    UProgressBar* CarryBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* CarryText;

    void UpdateStamina(float Percent);
    void UpdateCarry(int32 CurrentItems, int32 MaxItems);
};