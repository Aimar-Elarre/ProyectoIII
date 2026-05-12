#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DSaveGame.generated.h"

class UItemData;

/**
 * Struct for serializing inventory items to save file
 * Uses soft reference to avoid circular dependencies with ItemData
 */
USTRUCT(BlueprintType)
struct FInventorySlotSave
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSoftObjectPath ItemDataPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector DropScale = FVector(1.f, 1.f, 1.f);
};

/**
 * Save game class that handles persistent storage of:
 * - Checkpoint location
 * - Inventory state
 * - Current level name
 */
UCLASS()
class DUNGEONCROWLER_API UDSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UDSaveGame();

	// Checkpoint data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	FVector CheckpointLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	FRotator CheckpointRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	bool bHasCheckpoint = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
	FString CurrentLevelName;

	// Inventory data
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventorySlotSave> InventoryItems;

	// Metadata
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save")
	FDateTime SaveDateTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save")
	FString SaveSlotName = TEXT("AutoCheckpointSlot");

	// Save slot index (always 0 for auto-save)
	UPROPERTY()
	int32 UserIndex = 0;
};
