#include "InventorySlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "ItemData.h"
#include "InventoryComponent.h"

void UInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (DropButton)
    {
        DropButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnDropClicked);
    }
}

void UInventorySlotWidget::InitSlot(const FInventoryEntry& Entry, UInventoryComponent* InInventoryComp)
{
    CachedEntry = Entry;
    InventoryComp = InInventoryComp;

    if (!Entry.ItemData) return;

    if (ItemNameText)
    {
        ItemNameText->SetText(Entry.ItemData->DisplayName);
    }

    if (QuantityText)
    {
        QuantityText->SetText(FText::AsNumber(Entry.Quantity));
    }

    if (WeightText)
    {
        const float TotalWeight = Entry.Quantity * Entry.ItemData->WeightPerUnit;
        WeightText->SetText(FText::AsNumber(TotalWeight));
    }

    if (ItemIcon && Entry.ItemData->Icon)
    {
        ItemIcon->SetBrushFromTexture(Entry.ItemData->Icon);
    }
}

void UInventorySlotWidget::OnDropClicked()
{
    if (!InventoryComp || !CachedEntry.ItemData) return;

    InventoryComp->RemoveItem(CachedEntry.ItemData, 1);
}