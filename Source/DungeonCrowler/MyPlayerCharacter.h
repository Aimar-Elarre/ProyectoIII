#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "MyPlayerHUD.h"
#include "InventoryComponent.h"
#include "MyPlayerCharacter.generated.h"

class APickupItemActor;
class UInventoryWidget;
class UItemData;

UCLASS()
class DUNGEONCROWLER_API AMyPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyPlayerCharacter();

protected:
    virtual void BeginPlay() override;
    UPROPERTY()
    TObjectPtr<APickupItemActor> NearbyPickup = nullptr;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Movimiento
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
    void Dash();
    void ResetDash();
    void UpdateMovementSpeed();

    // Audio / IA
    void UpdateFootstepAudio(float ForwardValue);
    void StopFootstepAudio();
    void MakeMovementNoise(float Loudness);

    // Vida / Respawn
    void Die();
    void KillPlayer();
    void SetLastCheckpoint(FVector NewLocation);
    void RespawnAtCheckpoint();
    void TakeDamageCustom(float DamageAmount);


    UFUNCTION(BlueprintCallable, Category = "Pickup")
    void TryInteractPickup();

    void SetNearbyPickup(APickupItemActor* NewPickup);
    void ClearNearbyPickup(APickupItemActor* PickupToClear);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void DropSpecificItem(const UItemData* ItemData);
    // Sprint
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Run")
    bool bSprintUnlocked = false;

    UFUNCTION(BlueprintCallable, Category = "Run")
    bool IsSprintUnlocked() const;

    UFUNCTION(BlueprintCallable, Category = "Run")
    void UnlockSprint();

    // Dash
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Dash")
    bool bDashUnlocked = false;

    UFUNCTION(BlueprintCallable, Category = "Dash")
    bool IsDashUnlocked() const;

    UFUNCTION(BlueprintCallable, Category = "Dash")
    void UnlockDash();

    // Inventario desbloqueable
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
    bool bInventoryUnlocked = false;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool IsInventoryUnlocked() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void UnlockInventory();

    // Hints
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ShowHintMessage(const FString& Message);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void HideHintMessage();

    // Inventario
    void DropItem();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddCarriedItem(int32 Amount = 1);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetItemsCarried() const;

    UFUNCTION(BlueprintPure, Category = "Stats")
    float GetCurrentHealthValue() const;

    UFUNCTION(BlueprintPure, Category = "Stamina")
    float GetStaminaPercent() const;

    // Debug
    UFUNCTION(BlueprintCallable, Category = "Debug")
    void Debug_UnlockSprint();

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void Debug_UnlockDash();

    UFUNCTION(BlueprintCallable, Category = "Debug")
    void Debug_FillStamina();

    // Inventario nuevo
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UInventoryComponent> InventoryComponent = nullptr;

    // Slide
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slide")
    bool bIsSliding = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float SlideImpulse = 1200.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float SlideFriction = 0.05f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float SlideDuration = 0.75f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slide")
    float OriginalGroundFriction = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slide")
    float MinSlideSpeed = 350.f;
    int32 LastItemsCarriedForMovement = -1;
    FTimerHandle SlideTimerHandle;

    // Checkpoint / Respawn
    FTimerHandle RespawnTimerHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    bool bHasCheckpoint = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    bool bIsDead = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint")
    FVector LastCheckpointLocation = FVector::ZeroVector;

    // Dash
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
    float DashStrength = 2000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
    float DashCooldown = 1.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
    bool bCanDash = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
    float DashFOVBoost = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
    float DashFOVRecoverSpeed = 10.f;

    float CurrentDashFOVOffset = 0.f;
    FTimerHandle DashCooldownHandle;

// DINERO / MONEY
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	float CurrentMoney = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void AddMoney(float Amount);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	float GetCurrentMoney() const;

	// Crouch
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crouch")
    bool bIsCrouching = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float CrouchHeight = 44.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float StandingHeight = 88.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float CrouchSpeed = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float MeshStandingZ = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crouch")
    float MeshCrouchingZ = -25.f;

    float CurrentMeshZ = 0.f;
    float CurrentCapsuleHeight = 88.f;

    // Movement
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float WalkSpeed = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float RunSpeed = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
    float JumpStrength = 600.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsRunning = false;

    bool bRunKeyHeld = false;
    bool bHasJumped = false;

    // Cámara / Mouse
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse")
    float MouseSensitivity = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float NormalFOV = 90.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float RunFOV = 98.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    float FOVInterpSpeed = 8.f;

    // Stamina
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float MaxStamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
    float CurrentStamina = 100.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
    float StaminaDrainRate = 30.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
    float StaminaRegenRate = 20.f;

    // Vida
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth = 0.f;

    // Inventario / peso legacy temporal
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 ItemsCarried = 0;

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
    TObjectPtr<UMyPlayerHUD> PlayerHUD = nullptr;

    // Cámara
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<USpringArmComponent> SpringArm = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UCameraComponent> Camera = nullptr;

    // Audio
    UPROPERTY(EditAnywhere, Category = "Audio")
    TObjectPtr<USoundBase> FootstepSound = nullptr;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TObjectPtr<USoundBase> JumpSound = nullptr;

    UPROPERTY(EditAnywhere, Category = "Audio")
    TObjectPtr<USoundBase> DashSound = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    TObjectPtr<UAudioComponent> FootstepAudioComponent = nullptr;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float WalkStepInterval = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float RunStepInterval = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float FootstepBlockAfterJump = 0.25f;

    float LastFootstepTime = 0.f;
    float FootstepBlockedUntil = 0.f;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> InventoryWidgetClass;

    UPROPERTY()
    TObjectPtr<UUserWidget> InventoryWidgetInstance = nullptr;

    bool bInventoryOpen = false;

    FTimerHandle HintTimerHandle;
    FTimerHandle InventoryTutorialSecondHintHandle;

    void Input_Inventory_Toggle();
    void ShowInventory();
    void HideInventory();
    void RefreshLegacyCarryFromInventory();

    UFUNCTION()
    void ShowInventorySecondHint();
};