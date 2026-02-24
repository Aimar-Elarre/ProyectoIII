#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONCRAWLER_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    float MaxWeight = 50.0f;

    UPROPERTY(BlueprintAssignable)
    FOnInventoryChanged OnInventoryChanged;

    UFUNCTION(BlueprintCallable)
    bool AddItem(UItemData* ItemData, int32 Quantity);

    UFUNCTION(BlueprintCallable)
    float GetCurrentWeight() const;

    UFUNCTION(BlueprintCallable)
    void GetEntries(TArray<FInventoryEntry>& OutEntries) const;

private:
    UPROPERTY()
    TMap<FName, FInventoryEntry> Items;
};