#include "InventoryUnlockTrigger.h"

#include "Components/BoxComponent.h"
#include "MyPlayerCharacter.h"

AInventoryUnlockTrigger::AInventoryUnlockTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AInventoryUnlockTrigger::BeginPlay()
{
    Super::BeginPlay();

    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AInventoryUnlockTrigger::OnOverlapBegin);
    }
}

void AInventoryUnlockTrigger::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
    if (!Player)
    {
        return;
    }

    if (!Player->IsInventoryUnlocked())
    {
        Player->UnlockInventory();
    }

    Destroy();
}