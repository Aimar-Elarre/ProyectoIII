#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "InventoryEntry.generated.h"

USTRUCT(BlueprintType)
struct FInventoryEntry
{
    GENERATED_BODY()

    // Qué objeto es
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<const UItemData> ItemData = nullptr;

    // Cuántos tienes
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 Quantity = 0;

    FInventoryEntry() {}

    FInventoryEntry(const UItemData* InItem, int32 InQuantity)
        : ItemData(InItem), Quantity(InQuantity)
    {
    }
};
