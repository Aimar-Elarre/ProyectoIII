#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

float UInventoryComponent::GetCurrentWeight() const
{
    float Total = 0.f;

    for (const auto& Pair : Items)
    {
        const FInventoryEntry& Entry = Pair.Value;

        if (Entry.ItemData)
        {
            Total += Entry.Quantity * Entry.ItemData->WeightPerUnit;
        }
    }

    return Total;
}

bool UInventoryComponent::AddItem(UItemData* ItemData, int32 Quantity)
{
    if (!ItemData || Quantity <= 0)
        return false;

    float NewWeight = GetCurrentWeight() + Quantity * ItemData->WeightPerUnit;

    if (NewWeight > MaxWeight)
        return false;

    FInventoryEntry* Found = Items.Find(ItemData->ItemId);

    if (Found)
    {
        Found->Quantity += Quantity;
    }
    else
    {
        Items.Add(ItemData->ItemId, FInventoryEntry(ItemData, Quantity));
    }

    OnInventoryChanged.Broadcast();
    return true;
}

void UInventoryComponent::GetEntries(TArray<FInventoryEntry>& OutEntries) const
{
    OutEntries.Reset();
    Items.GenerateValueArray(OutEntries);
}