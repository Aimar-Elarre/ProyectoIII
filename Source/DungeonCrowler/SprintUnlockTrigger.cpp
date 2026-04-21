#include "SprintUnlockTrigger.h"

#include "Components/BoxComponent.h"
#include "MyPlayerCharacter.h"

ASprintUnlockTrigger::ASprintUnlockTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASprintUnlockTrigger::BeginPlay()
{
    Super::BeginPlay();

    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASprintUnlockTrigger::OnOverlapBegin);
    }
}

void ASprintUnlockTrigger::OnOverlapBegin(
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

    if (!Player->IsSprintUnlocked())
    {
        Player->UnlockSprint();
    }

    Destroy();
}