#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UVerticalBox;
class UTextBlock;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
class DUNGEONCROWLER_API UInventoryWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Llama a esto desde el personaje al abrir el inventario
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitInventory(UInventoryComponent* InInventoryComp);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void RefreshInventory();

protected:
    virtual void NativeConstruct() override;

    // Nombres deben coincidir con el WBP
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UVerticalBox> ItemList;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> CurrentWeightText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> MaxWeightText;

    // Clase del slot — asígnala en el WBP
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

private:
    UPROPERTY()
    TObjectPtr<UInventoryComponent> InventoryComp;

    UFUNCTION()
    void OnInventoryChanged();
};