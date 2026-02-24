// InventoryComponent.h
#pragma once
#include "Components/ActorComponent.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryEntry
{
    GENERATED_BODY()

    //UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    //TObjectPtr<const UItemData> ItemData = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TUJUEGO_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UInventoryComponent();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    float MaxWeight = 25.f;

    
    UPROPERTY(BlueprintAssignable)
    FOnInventoryChanged OnInventoryChanged;

    UFUNCTION(BlueprintCallable)
    float GetCurrentWeight() const;

    //UFUNCTION(BlueprintCallable)
    //bool AddItem(const UItemData* ItemData, int32 Quantity);

    UFUNCTION(BlueprintCallable)
    //const TMap<TObjectPtr<const UItemData>, FInventoryEntry>& GetItems() const { return Items; }

private:
    UPROPERTY()
    TMap<TObjectPtr<const UItemData>, FInventoryEntry> Items;
};