#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

class UTexture2D;
class APickupItemActor;

UCLASS(BlueprintType)
class DUNGEONCROWLER_API UItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "0.0"))
	float WeightPerUnit = 0.4f;

	//Qué actor se genera al soltar este objeto
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<APickupItemActor> PickupActorClass;
};