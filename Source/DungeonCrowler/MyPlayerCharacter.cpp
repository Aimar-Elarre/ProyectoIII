#include "MyPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);

    // Cámara FPS
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 0.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = true;
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;

    if (IsLocallyControlled())
    {
        UE_LOG(LogTemp, Warning, TEXT("WIDGET CREATED"));

        if (PlayerHUDClass)
        {
            PlayerHUD = CreateWidget<UMyPlayerHUD>(GetWorld(), PlayerHUDClass);

            if (PlayerHUD)
            {
                PlayerHUD->AddToViewport();
            }
        }
    }
}

void AMyPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsRunning)
    {
        CurrentStamina -= StaminaDrainRate * DeltaTime;

        if (CurrentStamina <= 0.f)
        {
            CurrentStamina = 0.f;
            StopRun();
        }
    }
    else
    {
        if (CurrentStamina < MaxStamina)
        {
            CurrentStamina += StaminaRegenRate * DeltaTime;
            CurrentStamina = FMath::Clamp(CurrentStamina, 0.f, MaxStamina);
        }
    }
    if (PlayerHUD)
    {
        PlayerHUD->UpdateStamina(GetStaminaPercent());
    }

    float TargetHeight = bIsCrouching ? CrouchHeight : StandingHeight;

    float CurrentHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

    float NewHeight = FMath::FInterpTo(
        CurrentHeight,
        TargetHeight,
        DeltaTime,
        CrouchSpeed
    );

    GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight, true);
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyPlayerCharacter::StopJump);
    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyPlayerCharacter::Dash);
    PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AMyPlayerCharacter::StartRun);
    PlayerInputComponent->BindAction("Run", IE_Released, this, &AMyPlayerCharacter::StopRun);
    PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyPlayerCharacter::StartCrouch);
    PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMyPlayerCharacter::StopCrouch);
    PlayerInputComponent->BindAction("Kill", IE_Pressed, this, &AMyPlayerCharacter::KillPlayer);
}

void AMyPlayerCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
        AddMovementInput(GetActorForwardVector(), Value);
}


void AMyPlayerCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorRightVector(), Value);
    }
}

void AMyPlayerCharacter::TakeDamageCustom(float DamageAmount)
{
    if (bIsDead) return;

    CurrentHealth -= DamageAmount;
    CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

    if (CurrentHealth <= 0.f)
    {
        Die();
    }
}


void AMyPlayerCharacter::StartJump()
{
    Jump();
}

void AMyPlayerCharacter::StopJump()
{
    StopJumping();
}
void AMyPlayerCharacter::Die()
{
    if (bIsDead) return;

    bIsDead = true;

    UE_LOG(LogTemp, Warning, TEXT("Personaje muerto"));

    GetCharacterMovement()->DisableMovement();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    GEngine->AddOnScreenDebugMessage(
        -1,
        2.f,
        FColor::Red,
        TEXT("ESTAS MUERTO")
    );

    // Esperar 2 segundos y respawnear
    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        this,
        &AMyPlayerCharacter::RespawnAtCheckpoint,
        2.0f,
        false
    );
}

void AMyPlayerCharacter::KillPlayer()
{
    if (bIsDead) return;

    CurrentHealth = 0.f;
    Die();
}

void AMyPlayerCharacter::SetLastCheckpoint(FVector NewLocation)
{
    LastCheckpointLocation = NewLocation;
    bHasCheckpoint = true;

    UE_LOG(LogTemp, Warning, TEXT("CHECKPOINT GUARDADO EN: %s"), *LastCheckpointLocation.ToString());
}
void AMyPlayerCharacter::RespawnAtCheckpoint()
{
    UE_LOG(LogTemp, Warning, TEXT("RESPAWN CHECKPOINT FLAG: %d"), bHasCheckpoint);

    if (!bHasCheckpoint)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            3.f,
            FColor::Red,
            TEXT("NO HAY CHECKPOINT GUARDADO")
        );
        return;
    }

    SetActorLocation(LastCheckpointLocation);

    CurrentHealth = MaxHealth;
    bIsDead = false;

    // Volver a activar movimiento
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // Volver a activar input
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        EnableInput(PC);
    }
}
void AMyPlayerCharacter::StartSlide()
{
    if (bIsSliding) return;

    bIsSliding = true;

    OriginalGroundFriction = GetCharacterMovement()->GroundFriction;
    GetCharacterMovement()->GroundFriction = 0.05f;

    FVector SlideVelocity = GetVelocity();
    SlideVelocity.Z = 0.f;

    LaunchCharacter(SlideVelocity * 1.2f, true, false);

    GetWorldTimerManager().SetTimer(
        SlideTimerHandle,
        this,
        &AMyPlayerCharacter::StopSlide,
        1.0f,
        false
    );
}
void AMyPlayerCharacter::StopSlide()
{
    bIsSliding = false;
    bIsCrouching = true;

    GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
}
void AMyPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AMyPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AMyPlayerCharacter::StartRun()
{
    if (bIsSliding) return;
    if (bIsCrouching) return;          // 🚫 No sprint si está agachado
    if (CurrentStamina <= 0.f) return;

    bIsRunning = true;
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}



void AMyPlayerCharacter::StopRun()
{
    bIsRunning = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}
float AMyPlayerCharacter::GetStaminaPercent() const
{
    return CurrentStamina / MaxStamina;
}
void AMyPlayerCharacter::StartCrouch()
{
    FVector HorizontalVelocity = GetVelocity();
    HorizontalVelocity.Z = 0.f;

    if (HorizontalVelocity.Size() > MinSlideSpeed && GetCharacterMovement()->IsMovingOnGround())
    {
        StartSlide();
        return;
    }

    bIsCrouching = true;
}
void AMyPlayerCharacter::StopCrouch()
{
    bIsCrouching = false;
}
void AMyPlayerCharacter::Dash()
{
    if (!bCanDash) return;

    bCanDash = false;

    const FRotator ControlRot = Controller ? Controller->GetControlRotation() : GetActorRotation();
    FVector DashDir = FRotationMatrix(ControlRot).GetUnitAxis(EAxis::X);
    DashDir.Z = 0.f;
    DashDir.Normalize();

    LaunchCharacter(DashDir * DashStrength, true, false);

    GetWorldTimerManager().SetTimer(
        SlideTimerHandle,
        this,
        &AMyPlayerCharacter::RespawnAtCheckpoint,
        2.0f,
        false
    );
}

