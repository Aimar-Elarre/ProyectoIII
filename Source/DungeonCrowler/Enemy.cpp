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
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetActor) return;

    AAIController* AI = Cast<AAIController>(GetController());
    if (AI)
    {
        AI->MoveToActor(TargetActor, 100.f);
    }

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());

    if (DistanceToPlayer <= KillDistance)
    {
        AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
        if (Player)
        {
            Player->KillPlayer();
        }
    }
}