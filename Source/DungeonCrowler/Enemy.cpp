#include "Enemy.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerCharacter.h"

AEnemy::AEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AEnemy::BeginPlay()
{
    Super::BeginPlay();

    TargetActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    InitialLocation = GetActorLocation();
    InitialRotation = GetActorRotation();
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetActor) return;

    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
    if (!Player) return;

    AAIController* AI = Cast<AAIController>(GetController());
    if (!AI) return;

    if (Player->bIsDead)
    {
        AI->StopMovement();
        SetActorLocation(InitialLocation);
        SetActorRotation(InitialRotation);
        return;
    }

    AI->MoveToActor(TargetActor, 100.f);

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());

    if (DistanceToPlayer <= KillDistance && !Player->bIsDead)
    {
        Player->KillPlayer();
        AI->StopMovement();
    }
}