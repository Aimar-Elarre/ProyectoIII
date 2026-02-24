// InventoryComponent.cpp
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
        const FInventoryEntry& E = Pair.Value;
        if (E.ItemData)
        {
            Total += E.Quantity * E.ItemData->WeightPerUnit;
        }
    }
    return Total;
}

bool UInventoryComponent::AddItem(const UItemData* ItemData, int32 Quantity)
{
    if (!ItemData || Quantity <= 0) return false;

    const float CurrentWeight = GetCurrentWeight();
    const float AddedWeight = Quantity * ItemData->WeightPerUnit;

    if (CurrentWeight + AddedWeight > MaxWeight)
    {
        return false;
    }

    FInventoryEntry& Entry = Items.FindOrAdd(ItemData);
    Entry.ItemData = ItemData;
    Entry.Quantity += Quantity;

    OnInventoryChanged.Broadcast();
    return true;
}