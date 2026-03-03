#include "MyPlayerHUD.h"

void UMyPlayerHUD::UpdateStamina(float Percent)
{
    UE_LOG(LogTemp, Warning, TEXT("Stamina Percent: %f"), Percent);
    if (StaminaBar)
    {
        StaminaBar->SetPercent(Percent);
    }
}