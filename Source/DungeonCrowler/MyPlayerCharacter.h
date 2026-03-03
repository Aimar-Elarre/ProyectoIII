#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/UserWidget.h"
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
    void Die();
    void KillPlayer();
    // SLIDE
    bool bIsSliding = false;
    float OriginalGroundFriction;
    float MinSlideSpeed = 350.f;
    bool bIsDead = false;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float SlideImpulse = 1200.f;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float SlideFriction = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Slide")
    float SlideDuration = 0.75f;

    FTimerHandle SlideTimerHandle;
    // DASH
    void Dash();

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashStrength = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashCooldown = 1.f;

    bool bCanDash = true;
    // CROUCH SUAVE
    bool bIsCrouching = false;

    UPROPERTY(EditAnywhere, Category = "Crouch")
    float CrouchHeight = 44.f;

    UPROPERTY(EditAnywhere, Category = "Crouch")
    float StandingHeight = 88.f;

    UPROPERTY(EditAnywhere, Category = "Crouch")
    float CrouchSpeed = 8.f;
    FTimerHandle DashCooldownHandle;

    // VELOCIDADES
    UPROPERTY(EditAnywhere, Category = "Movement")
    float WalkSpeed = 400.f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float RunSpeed = 800.f;

    bool bIsRunning = false;

    // STAMINA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
    float MaxStamina = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
    float CurrentStamina;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaDrainRate = 30.f;

    UPROPERTY(EditAnywhere, Category = "Stamina")
    float StaminaRegenRate = 20.f;

    // VIDA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> PlayerHUDClass;

    UMyPlayerHUD* PlayerHUD;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    void TakeDamageCustom(float DamageAmount);

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    UFUNCTION(BlueprintPure)
    float GetStaminaPercent() const;
};