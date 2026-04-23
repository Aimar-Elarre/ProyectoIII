#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UItemData;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Inactive UMETA(DisplayName = "Inactive"),
    Active UMETA(DisplayName = "Active")
};

UCLASS()
class DUNGEONCROWLER_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemy();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Chase")
    AActor* TargetActor = nullptr;

    UPROPERTY(EditAnywhere, Category = "Chase")
    float KillDistance = 200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation")
    float ActivationMoneyThreshold = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Activation")
    TObjectPtr<UItemData> TriggerItem = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Activation")
    EEnemyState CurrentState = EEnemyState::Inactive;

    UFUNCTION(BlueprintCallable, Category = "Activation")
    void CheckActivationCondition();

    UFUNCTION(BlueprintCallable, Category = "Activation")
    void ActivateEnemy();

protected:
    UPROPERTY()
    FVector InitialLocation = FVector::ZeroVector;

    UPROPERTY()
    FRotator InitialRotation = FRotator::ZeroRotator;
};