#include "InventorySlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "ItemData.h"
#include "MyPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.h"

void UInventorySlotWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (DropButton)
    {
        DropButton->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnDropClicked);
    }
}

void UInventorySlotWidget::InitSlot(const FInventoryEntry& Entry, UInventoryComponent* InInventoryComp)
{
    CachedEntry = Entry;
    InventoryComp = InInventoryComp;

    if (!Entry.ItemData)
    {
        return;
    }

    const float UnitWeight = Entry.ItemData->WeightPerUnit;
    const float TotalWeight = Entry.Quantity * UnitWeight;

    if (ItemNameText)
    {
        ItemNameText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("%s (%d)"),
                    *Entry.ItemData->DisplayName.ToString(),
                    Entry.Quantity
                )
            )
        );
    }

    if (ItemInfoText)
    {
        ItemInfoText->SetText(
            FText::FromString(
                FString::Printf(
                    TEXT("Peso (%.1f) / Total (%.1f)"),
                    UnitWeight,
                    TotalWeight
                )
            )
        );
    }

    if (ItemIcon && Entry.ItemData->Icon)
    {
        ItemIcon->SetBrushFromTexture(Entry.ItemData->Icon);
    }
}

void UInventorySlotWidget::OnDropClicked()
{
    if (!CachedEntry.ItemData)
    {
        return;
    }

    if (ClickSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), ClickSound);
    }

    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!Player)
    {
        return;
    }

    Player->DropSpecificItem(CachedEntry.ItemData);
}