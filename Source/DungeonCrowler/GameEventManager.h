#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameEventManager.generated.h"

class UUserWidget;
class AMyPlayerCharacter;
class UItemData;

UENUM(BlueprintType)
enum class EGameEventType : uint8
{
    None UMETA(DisplayName = "None"),
    GameStart UMETA(DisplayName = "Game Start"),
    EnemyActivation UMETA(DisplayName = "Enemy Activation"),
    GameEnd UMETA(DisplayName = "Game End")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameEventTriggered, EGameEventType, EventType);

UCLASS()
class DUNGEONCROWLER_API UGameEventManager : public UObject
{
    GENERATED_BODY()

public:
    static UGameEventManager& Get();

    // Inicializar con el jugador
    void Initialize(AMyPlayerCharacter* PlayerCharacter);

    // Verificar condiciones de activación del enemigo
    void CheckEnemyActivation();

    // Mostrar widget de evento
    void ShowEventWidget(EGameEventType EventType);

    // Configuración desde Blueprint
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Events")
    TSubclassOf<UUserWidget> StartWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Events")
    TSubclassOf<UUserWidget> EndWidgetClass;

    // Configuración de activación
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Events")
    float ActivationMoneyThreshold = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Events")
    TObjectPtr<UItemData> ActivationTriggerItem = nullptr;

    // Delegate para notificar eventos
    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameEventTriggered OnGameEventTriggered;

    // Estado del juego
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events")
    bool bGameStarted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events")
    bool bInitialized = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events")
    bool bEnemyActivated = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events")
    bool bGameEnded = false;

protected:
    void CheckMoneyActivation();
    void CheckItemActivation();

    UPROPERTY()
    TObjectPtr<AMyPlayerCharacter> PlayerCharacter;

    UPROPERTY()
    TObjectPtr<UUserWidget> CurrentWidget;
};