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
#include "MyPlayerCharacter.generated.h"

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

    // MOVIMIENTO
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
    void TryPlayFootstep();
    void UpdateFootstepAudio(float ForwardValue);
    void StopFootstepAudio();
    void MakeMovementNoise(float Loudness);

    // VIDA / RESPAWN
    void Die();
    void KillPlayer();
    void TakeDamageCustom(float DamageAmount);
    void SetLastCheckpoint(FVector NewLocation);
    void RespawnAtCheckpoint();

    UFUNCTION(BlueprintPure)
    float GetStaminaPercent() const;

protected:
    // C�MARA
    UPROPERTY(VisibleAnywhere, Category = "Camera")
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere, Category = "Camera")
    UCameraComponent* Camera;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float NormalFOV = 90.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float RunFOV = 98.f;

    UPROPERTY(EditAnywhere, Category = "Camera")
    float FOVInterpSpeed = 8.f;

    // AUDIO
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* FootstepSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* JumpSound;

    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* DashSound;

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    UAudioComponent* FootstepAudioComponent;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float WalkStepInterval = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float RunStepInterval = 0.3f;

    UPROPERTY(EditAnywhere, Category = "Audio")
    float FootstepBlockAfterJump = 0.25f;

    float LastFootstepTime = 0.f;
    float FootstepBlockedUntil = 0.f;

    // MOVEMENT
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed = 600.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float RunSpeed = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float JumpStrength = 600.f;

    bool bIsRunning = false;

    // CROUCH
    bool bIsCrouching = false;

    UPROPERTY(EditAnywhere, Category = "Crouch")
    float CrouchHeight = 44.f;

    UPROPERTY(EditAnywhere, Category = "Crouch")
    float StandingHeight = 88.f;

    UPROPERTY(EditAnywhere, Category = "Crouch")
    float CrouchSpeed = 8.f;

    // SLIDE
    bool bIsSliding = false;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float SlideImpulse = 1200.f;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float SlideFriction = 0.05f;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float SlideDuration = 0.75f;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float MinSlideSpeed = 350.f;

    float OriginalGroundFriction = 8.f;
    FTimerHandle SlideTimerHandle;

    // DASH
    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashStrength = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashCooldown = 1.f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashFOVBoost = 8.f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashFOVRecoverSpeed = 10.f;

    float CurrentDashFOVOffset = 0.f;

    bool bCanDash = true;
    FTimerHandle DashCooldownHandle;

    // STAMINA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float MaxStamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
    float CurrentStamina = 0.f;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaDrainRate = 30.f;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaRegenRate = 20.f;

    // VIDA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth = 0.f;

    bool bIsDead = false;

    // CHECKPOINT
    bool bHasCheckpoint = false;
    FVector LastCheckpointLocation = FVector::ZeroVector;
    FTimerHandle RespawnTimerHandle;

    // INVENTARIO / PESO
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 ItemsCarried = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxItemsCarried = 8;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    float SpeedPenaltyPerItem = 0.05f;

    UPROPERTY(EditAnywhere, Category = "Inventory")
    float MinSpeedMultiplier = 0.4f;

    // UI
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDClass;

    UPROPERTY()
    UMyPlayerHUD* PlayerHUD = nullptr;
};