#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
    void MoveForward(float Value);
    void MoveRight(float Value);
    void StartJump();
    void StopJump();
    void Turn(float Value);
    void LookUp(float Value);
    void Dash();



    // VIDA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float MaxHealth = 100.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
    float CurrentHealth;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashStrength = 1200.f;

    UPROPERTY(EditAnywhere, Category = "Dash")
    float DashCooldown = 1.0f;

    bool bCanDash = true;

    FTimerHandle DashCooldownHandle;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* SpringArm;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* Camera;

    void TakeDamageCustom(float DamageAmount);
};
