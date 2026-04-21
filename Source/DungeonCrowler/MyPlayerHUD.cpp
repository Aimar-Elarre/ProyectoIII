#include "MyPlayerHUD.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Math/UnrealMathUtility.h"

void UMyPlayerHUD::UpdateStamina(float Percent)
{
    if (StaminaBar)
    {
        StaminaBar->SetPercent(Percent);
    }
}

void UMyPlayerHUD::SetStaminaVisibility(bool bVisible)
{
    ESlateVisibility NewVisibility = bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed;

    if (StaminaBar)
    {
        StaminaBar->SetVisibility(NewVisibility);
    }
}

void UMyPlayerHUD::UpdateCarry(int32 CurrentItems, int32 MaxItems)
{
    if (CarryText)
    {
        CarryText->SetText(FText::FromString(FString::Printf(TEXT("Carga: %d / %d"), CurrentItems, MaxItems)));
    }

    if (CarryBar)
    {
        float Percent = 0.f;

        if (MaxItems > 0)
        {
            Percent = FMath::Clamp((float)CurrentItems / (float)MaxItems, 0.f, 1.f);
        }

        CarryBar->SetPercent(Percent);

        if (Percent < 0.4f)
        {
            CarryBar->SetFillColorAndOpacity(FLinearColor::Green);
        }
        else if (Percent < 0.7f)
        {
            CarryBar->SetFillColorAndOpacity(FLinearColor::Yellow);
        }
        else if (Percent < 0.9f)
        {
            CarryBar->SetFillColorAndOpacity(FLinearColor(1.f, 0.5f, 0.f, 1.f));
        }
        else
        {
            CarryBar->SetFillColorAndOpacity(FLinearColor::Red);
        }
    }
}

void UMyPlayerHUD::ShowHint(const FString& Message)
{
    if (HintText)
    {
        HintText->SetText(FText::FromString(Message));
        HintText->SetVisibility(ESlateVisibility::Visible);
    }
}

void UMyPlayerHUD::HideHint()
{
    if (HintText)
    {
        HintText->SetText(FText::GetEmpty());
        HintText->SetVisibility(ESlateVisibility::Hidden);
    }
}