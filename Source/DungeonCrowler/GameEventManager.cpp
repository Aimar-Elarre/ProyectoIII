#include "GameEventManager.h"
#include "MyPlayerCharacter.h"
#include "InventoryComponent.h"
#include "ItemData.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Mapa estático de instancias por mundo
TMap<UWorld*, UGameEventManager*> UGameEventManager::Instances;

UGameEventManager::UGameEventManager()
{
    bGameStarted = false;
    bInitialized = false;
    bEnemyActivated = false;
    bGameEnded = false;
}

UGameEventManager::~UGameEventManager()
{
}

UGameEventManager& UGameEventManager::Get(UObject* WorldContext)
{
    UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull);
    if (!World)
    {
        World = GEngine->GetCurrentPlayWorld();
    }
    
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventManager] Get: No se pudo obtener el mundo"));
        static UGameEventManager FallbackInstance;
        return FallbackInstance;
    }

    // Buscar o crear instancia
    UGameEventManager*& Instance = Instances.FindOrAdd(World);
    if (!Instance)
    {
        Instance = NewObject<UGameEventManager>(World);
        Instance->AddToRoot(); // Evitar que se recolecte por garbage
        UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] Get: Nueva instancia creada para mundo"));
    }

    return *Instance;
}

void UGameEventManager::Reset()
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
        UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] CheckEnemyActivation: No inicializado o sin jugador"));
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

        // Cambiar modo de input a UI
        if (APlayerController* PC = PlayerCharacter->GetController<APlayerController>())
        {
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(NewWidget->TakeWidget());
            InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            UE_LOG(LogTemp, Warning, TEXT("[GameEventManager] ShowEventWidget: Modo de input cambiado a UI"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[GameEventManager] ShowEventWidget: Error al crear widget"));
    }
}