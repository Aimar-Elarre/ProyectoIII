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

    // DASH
    void Dash();

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashStrength = 1000.f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashCooldown = 1.f;

    bool bCanDash = true;
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