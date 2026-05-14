#include "DungeonGameState.h"
#include "../Player/MyPlayerCharacter.h"
#include "../Inventory/InventoryComponent.h"
#include "../Inventory/ItemData.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ADungeonGameState::ADungeonGameState()
{
    bGameStarted = false;
    bInitialized = false;
    bEnemyActivated = false;
    bGameEnded = false;
}

void ADungeonGameState::Reset()
{
    bGameStarted = false;
    bInitialized = false;
    bEnemyActivated = false;
    bGameEnded = false;
    PlayerCharacter = nullptr;
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromParent();
        CurrentWidget = nullptr;
    }
}

void ADungeonGameState::Initialize(AMyPlayerCharacter* PlayerChar)
{
    if (!PlayerChar)
    {
        UE_LOG(LogTemp, Error, TEXT("[DungeonGameState] Initialize: PlayerCharacter es NULO"));
        return;
    }

    PlayerCharacter = PlayerChar;
    bInitialized = true;
    bGameStarted = true;

    UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] Initialize: Juego iniciado"));

    // Mostrar widget de inicio si está configurado
    if (StartWidgetClass)
    {
        ShowEventWidget(EGameEventType::GameStart);
    }
}

void ADungeonGameState::CheckEnemyActivation()
{
    if (!bInitialized || !PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] CheckEnemyActivation: No inicializado o sin jugador"));
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

void ADungeonGameState::CheckMoneyActivation()
{
    if (bEnemyActivated || ActivationMoneyThreshold <= 0.f)
    {
        return;
    }

    const float CurrentMoney = PlayerCharacter->GetCurrentMoney();
    UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] CheckMoneyActivation: Dinero: %.2f / Umbral: %.2f"),
        CurrentMoney, ActivationMoneyThreshold);

    if (CurrentMoney >= ActivationMoneyThreshold)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] ¡ACTIVACIÓN POR DINERO!"));
        bEnemyActivated = true;
        OnGameEventTriggered.Broadcast(EGameEventType::EnemyActivation);
        ShowEventWidget(EGameEventType::EnemyActivation);
    }
}

void ADungeonGameState::CheckItemActivation()
{
    if (bEnemyActivated || !ActivationTriggerItem)
    {
        return;
    }

    if (!PlayerCharacter->InventoryComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] CheckItemActivation: Sin inventario"));
        return;
    }

    const TArray<FInventoryEntry> Items = PlayerCharacter->InventoryComponent->GetItemsAsArray();

    for (const FInventoryEntry& Entry : Items)
    {
        if (Entry.ItemData && Entry.ItemData == ActivationTriggerItem && Entry.Quantity > 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] ¡ACTIVACIÓN POR ITEM: %s!"), *ActivationTriggerItem->GetName());
            bEnemyActivated = true;
            OnGameEventTriggered.Broadcast(EGameEventType::EnemyActivation);
            ShowEventWidget(EGameEventType::EnemyActivation);
            return;
        }
    }
}

void ADungeonGameState::ShowEventWidget(EGameEventType EventType)
{
    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Error, TEXT("[DungeonGameState] ShowEventWidget: Sin jugador"));
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
        UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] ShowEventWidget: WidgetClass no configurado para evento %d"), (int32)EventType);
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

        UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] ShowEventWidget: Widget mostrado para evento %d"), (int32)EventType);

        // Cambiar modo de input a UI
        if (APlayerController* PC = PlayerCharacter->GetController<APlayerController>())
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(NewWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            UE_LOG(LogTemp, Warning, TEXT("[DungeonGameState] ShowEventWidget: Modo de input cambiado a UI"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[DungeonGameState] ShowEventWidget: Error al crear widget"));
    }
}