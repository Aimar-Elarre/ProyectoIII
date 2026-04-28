#include "GameEventTrigger.h"
#include "GameEventManager.h"
#include "Components/BoxComponent.h"
#include "MyPlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AGameEventTrigger::AGameEventTrigger()
{
    PrimaryActorTick.bCanEverTick = true;

    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    SetRootComponent(TriggerBox);

    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AGameEventTrigger::BeginPlay()
{
    Super::BeginPlay();

    // Obtener referencia al GameEventManager singleton
    EventManager = &UGameEventManager::Get();

    if (TriggerBox)
    {
        TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGameEventTrigger::OnOverlapBegin);
    }
}

void AGameEventTrigger::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (bOneTimeTrigger && bHasTriggered)
    {
        return;
    }

    AMyPlayerCharacter* Player = Cast<AMyPlayerCharacter>(OtherActor);
    if (!Player)
    {
        return;
    }

    // Inicializar el GameEventManager con el jugador si no está inicializado
    if (EventManager && !EventManager->bInitialized)
    {
        EventManager->Initialize(Player);
    }

    // Verificar las condiciones de activación (dinero o item)
    if (EventManager)
    {
        EventManager->CheckEnemyActivation();

        // Si el enemigo ya está activado, mostrar el widget
        if (EventManager->bEnemyActivated && !bHasTriggered)
        {
            bHasTriggered = true;
            ActivateEvent(Player);
        }
    }
}

void AGameEventTrigger::ActivateEvent(AMyPlayerCharacter* Player)
{
    UE_LOG(LogTemp, Warning, TEXT("[GameEventTrigger] Evento activado!"));

    // Llamar evento de Blueprint
    OnEventTriggered();

    // Mostrar widget si está configurado
    if (EventWidgetClass)
    {
        UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), EventWidgetClass);
        if (NewWidget)
        {
            NewWidget->AddToViewport();
            UE_LOG(LogTemp, Warning, TEXT("[GameEventTrigger] Widget mostrado"));

            // Pausar juego si está configurado
            if (bPauseGameOnActivate)
            {
                if (APlayerController* PC = Player->GetController<APlayerController>())
                {
                    PC->SetPause(true);
                    UE_LOG(LogTemp, Warning, TEXT("[GameEventTrigger] Juego pausado"));
                }
            }
        }
    }

    // Destruir si está configurado
    if (bDestroyAfterTrigger)
    {
        Destroy();
    }
}