#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

UCLASS(BlueprintType)
class YOURGAME_API UItemData : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ItemId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon = nullptr;

    // Peso de 1 unidad de este item
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WeightPerUnit = 1.0f;

    // Cuántos pueden apilarse por stack (si quieres stacks)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxStackSize = 99;
};
