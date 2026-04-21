#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UItemData;

// Estado del enemigo para determinar si persigue al jugador
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Inactive UMETA(DisplayName = "Inactivo"),
    Active UMETA(DisplayName = "Activo - Persiguiendo")
};

UCLASS()
class DUNGEONCROWLER_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemy();

protected:
    virtual void BeginPlay() override;
    FVector InitialLocation;
    FRotator InitialRotation;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Chase")
    AActor* TargetActor;

    UPROPERTY(EditAnywhere, Category = "Chase")
    float KillDistance = 200.f;

    // ====== ACTIVACIÓN DEL ENEMIGO ======

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activation")
    EEnemyState CurrentState = EEnemyState::Inactive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation", meta = (ClampMin = "0.0"))
    float ActivationMoneyThreshold = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation")
    TObjectPtr<UItemData> TriggerItem = nullptr;

    UFUNCTION(BlueprintCallable, Category = "Activation")
    void ActivateEnemy();

    UFUNCTION(BlueprintCallable, Category = "Activation")
    void CheckActivationCondition();

    UFUNCTION(BlueprintPure, Category = "Activation")
    bool IsActive() const { return CurrentState == EEnemyState::Active; }
};