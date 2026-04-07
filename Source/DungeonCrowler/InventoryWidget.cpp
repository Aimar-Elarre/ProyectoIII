#include "InventoryWidget.h"

#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "InventorySlotWidget.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryWidget::InitInventory(UInventoryComponent* InInventoryComp)
{
    InventoryComp = InInventoryComp;

    if (InventoryComp)
    {
        InventoryComp->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::OnInventoryChanged);
        RefreshInventory();
    }
}

void UInventoryWidget::RefreshInventory()
{
    if (!InventoryComp || !ItemList || !SlotWidgetClass) return;

    ItemList->ClearChildren();

    for (const FInventoryEntry& Entry : InventoryComp->GetItemsAsArray())
    {
        UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, SlotWidgetClass);
        if (SlotWidget)
        {
            SlotWidget->InitSlot(Entry, InventoryComp);
            ItemList->AddChild(SlotWidget);
        }
    }

    if (CurrentWeightText)
    {
        CurrentWeightText->SetText(FText::AsNumber(InventoryComp->GetCurrentWeight()));
    }

    if (MaxWeightText)
    {
        MaxWeightText->SetText(FText::AsNumber(InventoryComp->MaxWeight));
    }
}

void UInventoryWidget::OnInventoryChanged()
{
    RefreshInventory();
}


