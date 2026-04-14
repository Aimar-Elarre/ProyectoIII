#include "Enemy.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "MyPlayerCharacter.h"
#include "InventoryComponent.h"
#include "ItemData.h"

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

    // Chequear si el enemigo debe activarse
    if (CurrentState == EEnemyState::Inactive)
    {
        CheckActivationCondition();
        return; // No perseguir hasta activarse
    }

    // Si está activo, perseguir al jugador
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

void AEnemy::CheckActivationCondition()
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
    if (!Player) return;

    // Condición 1: El jugador tiene el item de trigger
    if (TriggerItem && Player->InventoryComponent)
    {
        TArray<FInventoryEntry> Items = Player->InventoryComponent->GetItemsAsArray();
        for (const FInventoryEntry& Entry : Items)
        {
            if (Entry.ItemData == TriggerItem && Entry.Quantity > 0)
            {
                ActivateEnemy();
                return;
            }
        }
    }

    // Condición 2: El jugador tiene suficiente dinero
    if (Player->GetCurrentMoney() >= ActivationMoneyThreshold)
    {
        ActivateEnemy();
        return;
    }
}

void AEnemy::ActivateEnemy()
{
    if (CurrentState == EEnemyState::Active) return; // Ya está activo

    CurrentState = EEnemyState::Active;
    UE_LOG(LogTemp, Warning, TEXT("¡ENEMIGO ACTIVADO! Comenzando persecución..."));
}