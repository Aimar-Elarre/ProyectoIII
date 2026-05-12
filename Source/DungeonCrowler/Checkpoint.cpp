#include "Checkpoint.h"
#include "MyPlayerCharacter.h"
#include "InventoryComponent.h"
#include "DSaveGame.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetBoxExtent(FVector(100.f));
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);

    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    TriggerBox->SetGenerateOverlapEvents(true);
}

void ACheckpoint::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(
        this,
        &ACheckpoint::OnOverlapBegin
    );
}

void ACheckpoint::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    UE_LOG(LogTemp, Warning, TEXT("OVERLAP DETECTADO"));

    if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor))
    {
        Player->SetLastCheckpoint(GetActorLocation(), GetActorRotation());
        UE_LOG(LogTemp, Warning, TEXT("CHECKPOINT GUARDADO"));

        // Crear y guardar el juego
        if (UDSaveGame* SaveGameObject = NewObject<UDSaveGame>())
        {
            // Guardar ubicación y rotación del checkpoint
            SaveGameObject->CheckpointLocation = GetActorLocation();
            SaveGameObject->CheckpointRotation = GetActorRotation();
            SaveGameObject->bHasCheckpoint = true;

            // Guardar nivel actual
            if (UWorld* World = GetWorld())
            {
                SaveGameObject->CurrentLevelName = World->GetMapName();
                SaveGameObject->CurrentLevelName.RemoveFromStart(World->StreamingLevelsPrefix);
            }

            // Guardar inventario
            if (UInventoryComponent* Inventory = Player->InventoryComponent)
            {
                TArray<FInventoryEntry> InventoryEntries = Inventory->GetItemsAsArray();
                
                for (const FInventoryEntry& Entry : InventoryEntries)
                {
                    if (Entry.ItemData)
                    {
                        FInventorySlotSave SlotData;
                        SlotData.ItemDataPath = FSoftObjectPath(Entry.ItemData);
                        SlotData.Quantity = Entry.Quantity;
                        SlotData.DropScale = Entry.DropScale;
                        SaveGameObject->InventoryItems.Add(SlotData);
                    }
                }
            }

            // Guardar la partida
            SaveGameObject->SaveDateTime = FDateTime::Now();
            UGameplayStatics::SaveGameToSlot(SaveGameObject, SaveGameObject->SaveSlotName, SaveGameObject->UserIndex);
            UE_LOG(LogTemp, Warning, TEXT("PARTIDA GUARDADA EN SLOT: %s"), *SaveGameObject->SaveSlotName);
        }
    }
}