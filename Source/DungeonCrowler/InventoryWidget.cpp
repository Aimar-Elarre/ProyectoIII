#include "InventoryWidget.h"
#include "InventoryComponent.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"

void UInventoryWidget::Init(UInventoryComponent* InInventory)
{
    Inventory = InInventory;
    Refresh();
}

void UInventoryWidget::Refresh()
{
    if (!Inventory) return;

    if (WeightText)
    {
        const float Cur = Inventory->GetCurrentWeight();
        const float Max = Inventory->MaxWeight;
        WeightText->SetText(FText::FromString(FString::Printf(TEXT("Peso: %.1f / %.1f"), Cur, Max)));
    }

    if (Grid)
    {
        Grid->ClearChildren();
    }
}