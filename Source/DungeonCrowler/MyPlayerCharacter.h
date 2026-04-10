#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerHUD.h"
#include "InventoryComponent.h"
#include "MyPlayerCharacter.generated.h"

class APickupItemActor;
class UInventoryWidget;

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

    // VIDA / RESPAWN
    void Die();
    void KillPlayer();
    void SetLastCheckpoint(FVector NewLocation);
    void RespawnAtCheckpoint();
    void TakeDamageCustom(float DamageAmount);

    // DASH
    void Dash();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dash")
    bool bDashUnlocked = false;

    UFUNCTION(BlueprintCallable, Category = "Dash")
    bool IsDashUnlocked() const;

    UFUNCTION(BlueprintCallable, Category = "Dash")
    void UnlockDash();

    // HINTS
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowHintMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideHintMessage();

    // DROP / INVENTARIO
    void DropItem();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddCarriedItem(int32 Amount = 1);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetItemsCarried() const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentHealthValue() const;

    void UpdateMovementSpeed();

    // INVENTARIO NUEVO
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent;

    // SLIDE
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slide")
    bool bIsSliding = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float SlideImpulse = 1200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float SlideFriction = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float SlideDuration = 0.75f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slide")
    float OriginalGroundFriction = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float MinSlideSpeed = 350.f;

    FTimerHandle SlideTimerHandle;

    // CHECKPOINT / RESPAWN
    FTimerHandle RespawnTimerHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    bool bHasCheckpoint = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    bool bIsDead = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    FVector LastCheckpointLocation = FVector::ZeroVector;

    // DASH
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
    float DashStrength = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
    float DashCooldown = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    bool bCanDash = true;

    FTimerHandle DashCooldownHandle;

    // CROUCH
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crouch")
    bool bIsCrouching = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float CrouchHeight = 44.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float StandingHeight = 88.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float CrouchSpeed = 8.f;

    // MOVEMENT
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float WalkSpeed = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float RunSpeed = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float JumpStrength = 600.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsRunning = false;

    // SENSIBILIDAD
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
    float MouseSensitivity = 1.f;

    // STAMINA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float MaxStamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
    float CurrentStamina = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
    float StaminaDrainRate = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
    float StaminaRegenRate = 20.f;

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetStaminaPercent() const;

    // VIDA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere)
    float CurrentHealth = 0.f;

    // INVENTARIO / PESO (legacy)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 ItemsCarried = 4;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    float SpeedPenaltyPerItem = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    float MinSpeedMultiplier = 0.4f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
    TSubclassOf<APickupItemActor> PickupItemClass;

    // UI
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDClass;

    UPROPERTY()
    UMyPlayerHUD* PlayerHUD = nullptr;

    // CÁMARA
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArm = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* Camera = nullptr;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> InventoryWidgetInstance = nullptr;

    bool bInventoryOpen = false;

    void Input_Inventory_Toggle();
    void ShowInventory();
    void HideInventory();
};