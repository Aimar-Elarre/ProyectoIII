#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameEventTrigger.generated.h"

class UUserWidget;
class AMyPlayerCharacter;
class UGameEventManager;

UCLASS()
class DUNGEONCROWLER_API AGameEventTrigger : public AActor
{
    GENERATED_BODY()

public:
    AGameEventTrigger();

protected:
    virtual void BeginPlay() override;

public:
    // Referencia al GameEventManager (se obtiene automáticamente)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Event")
    TObjectPtr<UGameEventManager> EventManager;

    // Widget de Blueprint que se mostrará al activar
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Event")
    TSubclassOf<UUserWidget> EventWidgetClass;

    // Si es true, pausa el juego al mostrar el widget
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Event")
    bool bPauseGameOnActivate = true;

    // Si es true, solo se activa una vez
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Event")
    bool bOneTimeTrigger = true;

    // Si es true, destruye el actor después de activarse
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Event")
    bool bDestroyAfterTrigger = false;

    // Evento de Blueprint llamado cuando se activa
    UFUNCTION(BlueprintImplementableEvent, Category = "Game Event")
    void OnEventTriggered();

private:
    UPROPERTY()
    TObjectPtr<UBoxComponent> TriggerBox;

    bool bHasTriggered = false;

    UFUNCTION()
    void OnOverlapBegin(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    void ActivateEvent(AMyPlayerCharacter* Player);
};