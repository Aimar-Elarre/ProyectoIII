#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerHUD.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class DUNGEONCROWLER_API UMyPlayerHUD : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateStamina(float Percent);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void SetStaminaVisibility(bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void UpdateCarry(int32 CurrentItems, int32 MaxItems);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowHint(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void HideHint();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> StaminaBar = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> CarryBar = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CarryText = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> HintText = nullptr;
};