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
    // Obtener instancia singleton
    static UGameEventManager& Get(UObject* WorldContext = nullptr);

    // Inicializar con el jugador
    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void Initialize(AMyPlayerCharacter* PlayerCharacter);

    // Verificar condiciones de activación del enemigo
    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void CheckEnemyActivation();

    // Mostrar widget de evento
    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void ShowEventWidget(EGameEventType EventType);

    // Reiniciar el manager
    UFUNCTION(BlueprintCallable, Category = "Game Events")
    void Reset();

    // ==================== PROPIEDADES ====================
    
    // Configuración de activación
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Events|Activation")
    float ActivationMoneyThreshold = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Events|Activation")
    TObjectPtr<UItemData> ActivationTriggerItem = nullptr;

    // Widgets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Events|Widgets")
    TSubclassOf<UUserWidget> StartWidgetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Events|Widgets")
    TSubclassOf<UUserWidget> EndWidgetClass;

    // Estado del juego
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events|State")
    bool bGameStarted = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events|State")
    bool bInitialized = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events|State")
    bool bEnemyActivated = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events|State")
    bool bGameEnded = false;

    // Referencia al jugador
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events|State")
    TObjectPtr<AMyPlayerCharacter> PlayerCharacter = nullptr;

    // Widget actual
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Events|State")
    TObjectPtr<UUserWidget> CurrentWidget = nullptr;

    // Delegate para notificar eventos
    UPROPERTY(BlueprintAssignable, Category = "Game Events")
    FOnGameEventTriggered OnGameEventTriggered;

protected:
    UGameEventManager();
    virtual ~UGameEventManager() override;

private:
    void CheckMoneyActivation();
    void CheckItemActivation();

    // Mapa de instancias por mundo
    static TMap<UWorld*, UGameEventManager*> Instances;
};