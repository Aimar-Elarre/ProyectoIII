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
    
    // ===== DEBUG: Confirmación de TargetActor =====
    if (TargetActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] BeginPlay: TargetActor encontrado: %s"), *TargetActor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] BeginPlay: ¡ERROR! No se encontró el jugador. TargetActor es NULO"));
    }
    
    // ===== DEBUG: Mostrar valores de activación =====
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
}

void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ===== BREAKPOINT 1: Aquí debería entrar CADA FRAME =====
    if (!TargetActor)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] Tick: TargetActor es NULO. El enemigo no puede inicializarse."));
        return;
    }

    // Chequear si el enemigo debe activarse
    if (CurrentState == EEnemyState::Inactive)
    {
        // ===== BREAKPOINT 2: Aquí entra mientras el enemigo está inactivo =====
        CheckActivationCondition();
        return; // No perseguir hasta activarse
    }

    // Si está activo, perseguir al jugador
    // ===== BREAKPOINT 7: Verificar persecución =====
    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Tick - Estado ACTIVO. Intentando perseguir..."));
    
    AController* CurrentController = GetController();
    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] GetController() retorna: %s"), CurrentController ? *CurrentController->GetClass()->GetName() : TEXT("NULO"));
    
    if (CurrentController)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Controller encontrado. Intentando Cast a AAIController..."));
    }
    
    AAIController* AI = Cast<AAIController>(GetController());
    if (AI)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Cast a AAIController EXITOSO. Llamando MoveToActor..."));
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Posición Enemigo: %s"), *GetActorLocation().ToString());
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] Posición Jugador: %s"), *TargetActor->GetActorLocation().ToString());
        
        // ===== BREAKPOINT 8: Verificar que el AIController posee el Pawn =====
        APawn* OwnedPawn = AI->GetPawn();
        if (OwnedPawn)
        {
            UE_LOG(LogTemp, Warning, TEXT("[ENEMY] AIController posee el Pawn: %s"), *OwnedPawn->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR CRÍTICO! AIController NO posee ningún Pawn. El enemigo no se puede mover."));
        }
        
        // Verificar que el Pawn es este enemigo
        if (OwnedPawn != this)
        {
            UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR! El AIController posee otro Pawn, no este enemigo."));
        }
        
        // Intentar mover
        AI->MoveToActor(TargetActor, 100.f);
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] MoveToActor() fue llamado. Verificando si es movimiento fue iniciado..."));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR! Cast a AAIController FALLÓ. El enemigo NO perseguirá."));
    }

    float DistanceToPlayer = FVector::Dist(GetActorLocation(), TargetActor->GetActorLocation());
    UE_LOG(LogTemp, Log, TEXT("[ENEMY] Distancia al jugador: %.2f"), DistanceToPlayer);

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
    // ===== BREAKPOINT 3: Verificar que el Cast funciona =====
    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(TargetActor);
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] CheckActivationCondition: ¡FALLO EN CAST! TargetActor no es un AMyPlayerCharacter"));
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] TargetActor es de tipo: %s"), *TargetActor->GetClass()->GetName());
        return;
    }

    // ===== DEBUG: Mostrar dinero actual del jugador =====
    float CurrentPlayerMoney = Player->GetCurrentMoney();
    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] CheckActivationCondition: Dinero del jugador: %.2f (Umbral: %.2f)"), 
        CurrentPlayerMoney, ActivationMoneyThreshold);

    // Condición 1: El jugador tiene el item de trigger
    if (TriggerItem && Player->InventoryComponent)
    {
        TArray<FInventoryEntry> Items = Player->InventoryComponent->GetItemsAsArray();
        
        // ===== DEBUG: Mostrar items en el inventario =====
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] CheckActivationCondition: Items en inventario: %d"), Items.Num());
        
        for (const FInventoryEntry& Entry : Items)
        {
            if (Entry.ItemData)
            {
                UE_LOG(LogTemp, Warning, TEXT("  - Item: %s (Cantidad: %d)"), *Entry.ItemData->GetName(), Entry.Quantity);
                
                // ===== BREAKPOINT 4: Comparación de items =====
                if (Entry.ItemData == TriggerItem && Entry.Quantity > 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[ENEMY] ¡ACTIVACIÓN POR ITEM DETECTADA! Item: %s"), *TriggerItem->GetName());
                    ActivateEnemy();
                    return;
                }
            }
        }
        
        // Debug: Item de trigger no encontrado
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] TriggerItem configurado pero NO encontrado en inventario: %s"), *TriggerItem->GetName());
    }
    else if (TriggerItem && !Player->InventoryComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("[ENEMY] ¡ERROR! TriggerItem está configurado pero InventoryComponent es NULO"));
    }

    // Condición 2: El jugador tiene suficiente dinero
    // ===== BREAKPOINT 5: Chequeo de dinero =====
    if (CurrentPlayerMoney >= ActivationMoneyThreshold)
    {
        UE_LOG(LogTemp, Warning, TEXT("[ENEMY] ¡ACTIVACIÓN POR DINERO DETECTADA! %.2f >= %.2f"), 
            CurrentPlayerMoney, ActivationMoneyThreshold);
        ActivateEnemy();
        return;
    }
    
    // Debug: Ambas condiciones fallaron
    UE_LOG(LogTemp, Log, TEXT("[ENEMY] Condiciones no cumplidas. Esperando..."));
}

void AEnemy::ActivateEnemy()
{
    if (CurrentState == EEnemyState::Active)
    {
        UE_LOG(LogTemp, Log, TEXT("[ENEMY] ActivateEnemy: ¡Ya estaba activo!"));
        return;
    }

    // ===== BREAKPOINT 6: Aquí debería llegar cuando se activa =====
    CurrentState = EEnemyState::Active;
    UE_LOG(LogTemp, Warning, TEXT("//////////////////////////////////////////////////////"));
    UE_LOG(LogTemp, Warning, TEXT("¡¡¡ ENEMIGO ACTIVADO !!! - Comenzando persecución..."));
    UE_LOG(LogTemp, Warning, TEXT("//////////////////////////////////////////////////////"));
}