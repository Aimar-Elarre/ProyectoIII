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

    InitialLocation = GetActorLocation();
    InitialRotation = GetActorRotation();

    TargetActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] BeginPlay: TargetActor encontrado: %s"), *TargetActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] BeginPlay: ¡ERROR! No se encontró el jugador. TargetActor es NULO"));
    }

    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Configuración de Activación:"));
    UE_LOG(LogTemp, Warning, TEXT("  - ActivationMoneyThreshold: %.2f"), ActivationMoneyThreshold);

    if (TriggerItem)
    {
        UE_LOG(LogTemp, Warning, TEXT("  - TriggerItem: %s"), *TriggerItem->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("  - TriggerItem: NONE (solo se activará por dinero)"));
    }

    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Posición inicial guardada: %s"), *InitialLocation.ToString());
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] Tick: TargetActor es NULO. El enemigo no puede inicializarse."));
        return;
    }

    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] Tick: TargetActor no es AMyPlayerCharacter."));
        return;
    }

    if (CurrentState == EEnemyState::Inactive)
    {
        CheckActivationCondition();
        return;
    }

    AAIController* AI = Cast<AAIController>(GetController());
    if (!AI)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR! Cast a AAIController FALLÓ. El enemigo NO perseguirá."));
        return;
    }

    if (Player->bIsDead)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] El jugador está muerto. Volviendo a posición inicial."));
        AI->StopMovement();
        SetActorLocation(InitialLocation);
        SetActorRotation(InitialRotation);
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Tick - Estado ACTIVO. Intentando perseguir..."));
    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Posición Enemigo: %s"), *GetActorLocation().ToString());
    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Posición Jugador: %s"), *TargetActor->GetActorLocation().ToString());

    APawn* OwnedPawn = AI->GetPawn();
    if (OwnedPawn)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] AIController posee el Pawn: %s"), *OwnedPawn->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR CRÍTICO! AIController NO posee ningún Pawn."));
        return;
    }

    if (OwnedPawn != this)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR! El AIController posee otro Pawn, no este enemigo."));
    }

    AI->MoveToActor(TargetActor, 100.f);

    const float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    UE_LOG(LogTemp, Log, TEXT("[ENEMY] Distancia al jugador: %.2f"), DistanceToPlayer);

    if (DistanceToPlayer <= KillDistance)
    {
        Player->KillPlayer();
        AI->StopMovement();
    }
}

void AEnemy::CheckActivationCondition()
{
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] CheckActivationCondition: ¡FALLO EN CAST! TargetActor no es un AMyPlayerCharacter"));
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] TargetActor es de tipo: %s"), *TargetActor->GetClass()->GetName());
        return;
    }

    const float CurrentPlayerMoney = Player->GetCurrentMoney();
    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] CheckActivationCondition: Dinero del jugador: %.2f (Umbral: %.2f)"),
        CurrentPlayerMoney, ActivationMoneyThreshold);

    if (TriggerItem && Player->InventoryComponent)
    {
        const TArray<FInventoryEntry> Items = Player->InventoryComponent->GetItemsAsArray();

        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] CheckActivationCondition: Items en inventario: %d"), Items.Num());

        for (const FInventoryEntry& Entry : Items)
        {
            if (Entry.ItemData)
            {
                UE_LOG(LogTemp, Warning, TEXT("  - Item: %s (Cantidad: %d)"), *Entry.ItemData->GetName(), Entry.Quantity);

                if (Entry.ItemData == TriggerItem && Entry.Quantity > 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] ¡ACTIVACIÓN POR ITEM DETECTADA! Item: %s"), *TriggerItem->GetName());
                    ActivateEnemy();
                    return;
                }
            }
        }

        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] TriggerItem configurado pero NO encontrado en inventario: %s"), *TriggerItem->GetName());
    }
    else if (TriggerItem && !Player->InventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR! TriggerItem está configurado pero InventoryComponent es NULO"));
    }

    if (CurrentPlayerMoney >= ActivationMoneyThreshold)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] ¡ACTIVACIÓN POR DINERO DETECTADA! %.2f >= %.2f"),
            CurrentPlayerMoney, ActivationMoneyThreshold);
        ActivateEnemy();
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[ENEMY] Condiciones no cumplidas. Esperando..."));
}

void AEnemy::ActivateEnemy()
{
    if (CurrentState == EEnemyState::Active)
    {
        UE_LOG(LogTemp, Log, TEXT("[ENEMY] ActivateEnemy: ¡Ya estaba activo!"));
        return;
    }

    CurrentState = EEnemyState::Active;

    UE_LOG(LogTemp, Warning, TEXT("//////////////////////////////////////////////////////"));
    UE_LOG(LogTemp, Warning, TEXT("¡¡¡ ENEMIGO ACTIVADO !!! - Comenzando persecución..."));
    UE_LOG(LogTemp, Warning, TEXT("//////////////////////////////////////////////////////"));
}