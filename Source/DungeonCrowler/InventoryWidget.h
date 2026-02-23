#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "InventoryWidget.generated.h"

class UUniformGridPanel;
class UTextBlock;
class UInventoryComponent;

UCLASS()
class YOURGAME_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void Init(UInventoryComponent* InInventory);

    UFUNCTION()
    void Refresh();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UUniformGridPanel> Grid;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> WeightText;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> SlotWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UInventoryComponent> Inventory;
};