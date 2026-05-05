#include "InventoryWidget.h"

#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "InventorySlotWidget.h"
#include "Components/ProgressBar.h"

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
        CurrentWeightText->SetText(
            FText::FromString(FString::Printf(TEXT("%.1f"), InventoryComp->GetCurrentWeight()))
        );
    }

    if (MaxWeightText)
    {
        MaxWeightText->SetText(
            FText::FromString(FString::Printf(TEXT("%.1f"), InventoryComp->MaxWeight))
        );
    }
    if (WeightBar && InventoryComp->MaxWeight > 0.f)
    {
        WeightBar->SetPercent(InventoryComp->GetCurrentWeight() / InventoryComp->MaxWeight);
    }
}

void UInventoryWidget::OnInventoryChanged()
{
    RefreshInventory();
}


