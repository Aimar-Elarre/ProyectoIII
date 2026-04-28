#include "GameEventManager.h"
#include "MyPlayerCharacter.h"
#include "InventoryComponent.h"
#include "ItemData.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"

UGameEventManager& UGameEventManager::Get()
{
    static UGameEventManager Instance;
    return Instance;
}

void UGameEventManager::Initialize(AMyPlayerCharacter* PlayerChar)
{
    if (!PlayerChar)
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventManager] Initialize: PlayerCharacter es NULO"));
        return;
    }

    PlayerCharacter = PlayerChar;
    bInitialized = true;
    bGameStarted = true;

    UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] Initialize: Juego iniciado"));

    // Mostrar widget de inicio si está configurado
    if (StartWidgetClass)
    {
        ShowEventWidget(EGameEventType::GameStart);
    }
}

void UGameEventManager::CheckEnemyActivation()
{
    if (!bInitialized || !PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventManager] CheckEnemyActivation: No inicializado"));
        return;
    }

    // Ya activado
    if (bEnemyActivated)
    {
        return;
    }

    // Verificar condición de dinero
    CheckMoneyActivation();

    // Verificar condición de item
    CheckItemActivation();
}

void UGameEventManager::CheckMoneyActivation()
{
    if (bEnemyActivated || ActivationMoneyThreshold <= 0.f)
    {
        return;
    }

    const float CurrentMoney = PlayerCharacter->GetCurrentMoney();
    UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] CheckMoneyActivation: Dinero: %.2f / Umbral: %.2f"),
        CurrentMoney, ActivationMoneyThreshold);

    if (CurrentMoney >= ActivationMoneyThreshold)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] ¡ACTIVACIÓN POR DINERO!"));
        bEnemyActivated = true;
        OnGameEventTriggered.Broadcast(EGameEventType::EnemyActivation);
        ShowEventWidget(EGameEventType::EnemyActivation);
    }
}

void UGameEventManager::CheckItemActivation()
{
    if (bEnemyActivated || !ActivationTriggerItem)
    {
        return;
    }

    if (!PlayerCharacter->InventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] CheckItemActivation: Sin inventario"));
        return;
    }

    const TArray<FInventoryEntry> Items = PlayerCharacter->InventoryComponent->GetItemsAsArray();

    for (const FInventoryEntry& Entry : Items)
    {
        if (Entry.ItemData && Entry.ItemData == ActivationTriggerItem && Entry.Quantity > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] ¡ACTIVACIÓN POR ITEM: %s!"), *ActivationTriggerItem->GetName());
            bEnemyActivated = true;
            OnGameEventTriggered.Broadcast(EGameEventType::EnemyActivation);
            ShowEventWidget(EGameEventType::EnemyActivation);
            return;
        }
    }
}

void UGameEventManager::ShowEventWidget(EGameEventType EventType)
{
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventManager] ShowEventWidget: Sin jugador"));
        return;
    }

    UClass* WidgetClass = nullptr;

    switch (EventType)
    {
    case EGameEventType::GameStart:
        WidgetClass = StartWidgetClass;
        break;
    case EGameEventType::EnemyActivation:
    case EGameEventType::GameEnd:
        WidgetClass = EndWidgetClass;
        break;
    default:
        return;
    }

    if (!WidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] ShowEventWidget: WidgetClass no configurado para evento %d"), (int32)EventType);
        return;
    }

    // Crear el widget
    UUserWidget* NewWidget = CreateWidget<UUserWidget>(PlayerCharacter->GetWorld(), WidgetClass);
    if (NewWidget)
    {
        // Cerrar widget anterior si existe
        if (CurrentWidget)
        {
            CurrentWidget->RemoveFromParent();
        }

        CurrentWidget = NewWidget;
        NewWidget->AddToViewport();

        UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] ShowEventWidget: Widget mostrado para evento %d"), (int32)EventType);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventManager] ShowEventWidget: Error al crear widget"));
    }
}