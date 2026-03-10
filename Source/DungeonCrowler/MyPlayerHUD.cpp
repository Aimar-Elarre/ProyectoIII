#include "MyPlayerHUD.h"

void UMyPlayerHUD::UpdateStamina(float Percent)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(Percent);
    }
}