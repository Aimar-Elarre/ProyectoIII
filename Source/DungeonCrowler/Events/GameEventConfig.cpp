#include "GameEventConfig.h"
#include "../Core/DungeonGameState.h"
#include "../Player/MyPlayerCharacter.h"

AGameEventConfig::AGameEventConfig()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGameEventConfig::BeginPlay()
{
    Super::BeginPlay();

    // Obtener referencia al GameState
    GameState = GetWorld()->GetGameState<ADungeonGameState>();
    if (!GameState)
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventConfig] BeginPlay: No se pudo obtener ADungeonGameState"));
        return;
    }

    // Configurar el threshold de dinero
    GameState->ActivationMoneyThreshold = ActivationMoneyThreshold;

    // Configurar el item de activación (si hay uno)
    if (ActivationTriggerItem)
    {
        GameState->ActivationTriggerItem = ActivationTriggerItem;
    }

    // Configurar los widgets
    GameState->StartWidgetClass = StartWidgetClass;
    GameState->EndWidgetClass = EndWidgetClass;

    // Opciones de debug
    if (bForceActivation)
    {
        GameState->bEnemyActivated = true;
    }

    if (DebugGiveMoney > 0.f)
    {
        // Dar dinero al jugador para pruebas
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            if (AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(PC->GetPawn()))
            {
                Player->AddMoney(DebugGiveMoney);
                UE_LOG(LogTemp, Warning, TEXT("[GameEventConfig] Dado dinero de debug: %.2f"), DebugGiveMoney);
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("[GameEventConfig] Configurado - Threshold: %.2f"), ActivationMoneyThreshold);
}