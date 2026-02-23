#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "InventoryTypes.generated.h"

USTRUCT(BlueprintType)
struct FInventoryEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UItemData> ItemData = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 Quantity = 0;

    FInventoryEntry() {}

    FInventoryEntry(UItemData* InData, int32 InQty)
        : ItemData(InData), Quantity(InQty)
    {
    }
};