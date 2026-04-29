#include "GameEventConfig.h"
#include "GameEventManager.h"
#include "MyPlayerCharacter.h"

AGameEventConfig::AGameEventConfig()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGameEventConfig::BeginPlay()
{
    Super::BeginPlay();

    // Obtener referencia al GameEventManager singleton
    EventManager = &UGameEventManager::Get(this);

    // Configurar el threshold de dinero
    EventManager->ActivationMoneyThreshold = ActivationMoneyThreshold;

    // Configurar el item de activaci�n (si hay uno)
    if (ActivationTriggerItem)
    {
        EventManager->ActivationTriggerItem = ActivationTriggerItem;
    }

    // Configurar los widgets
    EventManager->StartWidgetClass = StartWidgetClass;
    EventManager->EndWidgetClass = EndWidgetClass;

    // Opciones de debug
    if (bForceActivation)
    {
        EventManager->bEnemyActivated = true;
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