#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerHUD.h"
#include "MyPlayerCharacter.generated.h"

class APickupItemActor;

UCLASS()
class DUNGEONCROWLER_API AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyPlayerCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartRun();
    void StopRun();
    void StartJump();
    void StopJump();
    void Turn(float Value);
    void LookUp(float Value);
    void StartCrouch();
    void StopCrouch();
    void StartSlide();
    void StopSlide();

    void Die();
    void DropItem();
    void RespawnAtCheckpoint();
    void TakeDamageCustom(float DamageAmount);

    UFUNCTION(BlueprintCallable)
    void SetLastCheckpoint(FVector NewLocation);

    void AddCarriedItem(int32 Amount = 1);
    int32 GetItemsCarried() const;
    float GetCurrentHealthValue() const;

    UFUNCTION(BlueprintCallable)
    bool IsDashUnlocked() const;

    UFUNCTION(BlueprintCallable)
    void UnlockDash();

    UFUNCTION(BlueprintCallable)
    void ShowHintMessage(const FString& Message);

    UFUNCTION(BlueprintCallable)
    void HideHintMessage();

    UPROPERTY(VisibleAnywhere)
    bool bDashUnlocked = false;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    int32 ItemsCarried = 4;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    float SpeedPenaltyPerItem = 0.05f;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    float MinSpeedMultiplier = 0.4f;

    void UpdateMovementSpeed();

    UPROPERTY(EditAnywhere, Category = "Inventory")
    TSubclassOf<APickupItemActor> PickupItemClass;

    UPROPERTY(EditAnywhere)
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere)
    float CurrentHealth = 0.f;

    bool bHasCheckpoint = false;
    FVector LastCheckpointLocation;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    UFUNCTION(BlueprintPure)
    float GetStaminaPercent() const;
};