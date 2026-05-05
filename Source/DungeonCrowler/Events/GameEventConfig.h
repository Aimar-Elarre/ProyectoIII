#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameEventConfig.generated.h"

class UGameEventManager;
class UItemData;
class UUserWidget;

UCLASS()
class DUNGEONCROWLER_API AGameEventConfig : public AActor
{
    GENERATED_BODY()

public:
    AGameEventConfig();

protected:
    virtual void BeginPlay() override;

public:
    // ==================== CONFIGURACIÓN DE ACTIVACIÓN ====================

    // Cantidad de dinero necesaria para activar el evento
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Activation")
    float ActivationMoneyThreshold = 0.f;

    // Item específico que activa el evento (opcional)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Activation")
    TObjectPtr<UItemData> ActivationTriggerItem = nullptr;

    // ==================== WIDGETS ====================

    // Widget de inicio del juego
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Widgets")
    TSubclassOf<UUserWidget> StartWidgetClass;

    // Widget de activación (cuando se activa el enemigo)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Widgets")
    TSubclassOf<UUserWidget> EndWidgetClass;

    // ==================== OPCIONES ====================

    // Pausar el juego cuando se muestre un widget
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Options")
    bool bPauseGameOnActivate = true;

    // Mostrar widget de inicio al comenzar
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Options")
    bool bShowStartWidget = false;

    // Forzar activación inmediata (para pruebas)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Debug")
    bool bForceActivation = false;

    // Forzar cantidad de dinero (para pruebas)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Event|Debug")
    float DebugGiveMoney = 0.f;

private:
    UPROPERTY()
    UGameEventManager* EventManager;
};