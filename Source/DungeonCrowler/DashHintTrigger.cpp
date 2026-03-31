#include "DashHintTrigger.h"
#include "MyPlayerCharacter.h"

ADashHintTrigger::ADashHintTrigger()
{
    PrimaryActorTick.bCanEverTick = false;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    TriggerBox->SetGenerateOverlapEvents(true);
}

void ADashHintTrigger::BeginPlay()
{
    Super::BeginPlay();

    TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ADashHintTrigger::OnPlayerEnter);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ADashHintTrigger::OnPlayerExit);
}

void ADashHintTrigger::OnPlayerEnter(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
    if (Player)
    {
        if (!Player->IsDashUnlocked())
        {
            Player->UnlockDash();
        }
        else
        {
            Player->ShowHintMessage(TEXT("Pulsa Q para usar el dash en el aire"));
        }
    }
}

void ADashHintTrigger::OnPlayerExit(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
    if (Player)
    {
        Player->HideHintMessage();
    }
}