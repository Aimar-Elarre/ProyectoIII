#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UTextBlock;
class UInventoryComponent;

UCLASS()
class DUNGEONCRAWLER_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void Init(UInventoryComponent* InInventory);

    UFUNCTION(BlueprintCallable)
    void Refresh();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> Grid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> WeightText;

private:
    UPROPERTY()
    TObjectPtr<UInventoryComponent> Inventory;
};