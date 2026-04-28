#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryComponent.h"
#include "InventorySlotWidget.generated.h"

class UTextBlock;
class UImage;
class UButton;
class UInventoryComponent;
class USoundBase;

UCLASS()
class DUNGEONCROWLER_API UInventorySlotWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void InitSlot(const FInventoryEntry& Entry, UInventoryComponent* InInventoryComp);

protected:
    virtual void NativeConstruct() override;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> ItemIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemNameText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> ItemInfoText;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> DropButton;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TObjectPtr<USoundBase> ClickSound = nullptr;

private:
    UFUNCTION()
    void OnDropClicked();

    FInventoryEntry CachedEntry;

    UPROPERTY()
    TObjectPtr<UInventoryComponent> InventoryComp;
};