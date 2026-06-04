#include "Enemy.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "../Player/MyPlayerCharacter.h"
#include "../Inventory/InventoryComponent.h"
#include "../Inventory/ItemData.h"

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
    AIController = Cast<AAIController>(GetController());
    bHasMoveCommand = false;
    ActivationQueryTimer = 0.f;

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
        return;
    }

    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
    if (!Player)
    {
        return;
    }

    if (CurrentState == EEnemyState::Inactive)
    {
        ActivationQueryTimer += DeltaTime;
        if (ActivationQueryTimer >= ActivationCheckInterval)
        {
            ActivationQueryTimer = 0.f;
            CheckActivationCondition();
        }
        return;
    }

    if (!AIController)
    {
        AIController = Cast<AAIController>(GetController());
        if (!AIController)
        {
            return;
        }
    }

    if (Player->bIsDead)
    {
        AIController->StopMovement();
        SetActorLocation(InitialLocation);
        SetActorRotation(InitialRotation);
        return;
    }

    if (!bHasMoveCommand)
    {
        AIController->MoveToActor(TargetActor, 100.f);
        bHasMoveCommand = true;
    }

    const float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    if (DistanceToPlayer <= KillDistance)
    {
        Player->KillPlayer();
        AIController->StopMovement();
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
        return;
    }

    CurrentState = EEnemyState::Active;
    bHasMoveCommand = false;
}