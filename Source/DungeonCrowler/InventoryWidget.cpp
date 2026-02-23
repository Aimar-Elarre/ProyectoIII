#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "InventoryComponent.h"
#include "Blueprint/WidgetTree.h"

void UInventoryWidget::Init(UInventoryComponent* InInventory)
{
    Inventory = InInventory;
    if (Inventory)
    {
        Inventory->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::Refresh);
    }
    Refresh();
}

void UInventoryWidget::Refresh()
{
    if (!Inventory || !Grid || !WeightText) return;

    Grid->ClearChildren();

    const float CurW = Inventory->GetCurrentWeight();
    const float MaxW = Inventory->MaxWeight;
    WeightText->SetText(FText::FromString(FString::Printf(TEXT("Peso: %.1f / %.1f"), CurW, MaxW)));

    TArray<FInventoryEntry> Entries;
    Inventory->GetEntries(Entries);

    // Orden opcional (por nombre)
    Entries.Sort([](const FInventoryEntry& A, const FInventoryEntry& B)
        {
            const FString AN = A.ItemData ? A.ItemData->DisplayName.ToString() : "";
            const FString BN = B.ItemData ? B.ItemData->DisplayName.ToString() : "";
            return AN < BN;
        });

    const int32 Columns = 4;
    for (int32 i = 0; i < Entries.Num(); i++)
    {
        if (!SlotWidgetClass) break;

        UUserWidget* Slot = CreateWidget<UUserWidget>(GetWorld(), SlotWidgetClass);
        if (!Slot) continue;

        // Aquí lo típico es que tu SlotWidget tenga una función SetData(ItemData, Qty).
        // Si quieres, te doy el código del SlotWidget C++ también.
        // Por ejemplo:
        // Cast<UInventorySlotWidget>(Slot)->SetData(Entries[i]);

        const int32 Row = i / Columns;
        const int32 Col = i % Columns;
        Grid->AddChildToUniformGrid(Slot, Row, Col);
    }
}