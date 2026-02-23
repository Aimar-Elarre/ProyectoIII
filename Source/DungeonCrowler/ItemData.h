#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

UCLASS(BlueprintType)
class YOURGAME_API UItemData : public UDataAsset
{
    GENERATED_BODY()

public:
    // ID único del objeto (Potion, Key, Ammo...)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ItemId;

    // Nombre que se muestra en el inventario
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    // Imagen del objeto
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UTexture2D* Icon = nullptr;

    // Peso de UNA unidad
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WeightPerUnit = 1.0f;

    // Máximo que se puede acumular
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 MaxStackSize = 99;
};
