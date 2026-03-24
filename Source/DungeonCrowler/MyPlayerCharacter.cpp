#include "MyPlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"


AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCapsuleComponent()->SetGenerateOverlapEvents(true);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 0.f;
    SpringArm->bUsePawnControlRotation = true;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));

    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    GetCharacterMovement()->JumpZVelocity = 420.f;
    GetCharacterMovement()->GravityScale = 1.6f;
    GetCharacterMovement()->AirControl = 0.5f;
    GetCharacterMovement()->FallingLateralFriction = 1.0f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

    JumpMaxHoldTime = 0.0f;

    CurrentCapsuleHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

    FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComponent"));
    FootstepAudioComponent->SetupAttachment(RootComponent);
    FootstepAudioComponent->bAutoActivate = false;
    FootstepAudioComponent->bIsUISound = false;
}
void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
    CurrentStamina = MaxStamina;

    GetCharacterMovement()->JumpZVelocity = JumpStrength;
    UpdateMovementSpeed();

    if (Camera)
    {
        Camera->SetFieldOfView(NormalFOV);
    }

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (PC->PlayerCameraManager)
        {
            PC->PlayerCameraManager->ViewPitchMin = -40.f;
            PC->PlayerCameraManager->ViewPitchMax = 70.f;
        }
    }

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

    if (FootstepAudioComponent && FootstepSound)
    {
        FootstepAudioComponent->SetSound(FootstepSound);
    }
}
void AMyPlayerCharacter::UpdateFootstepAudio(float ForwardValue)
{
    if (!FootstepAudioComponent || !FootstepSound) return;

    bool bMovingForwardOrBackward = FMath::Abs(ForwardValue) > 0.01f;
    bool bCanPlay =
        bMovingForwardOrBackward &&
        GetCharacterMovement()->IsMovingOnGround() &&
        !bIsSliding &&
        !bIsDead;

    if (bCanPlay)
    {
        if (!FootstepAudioComponent->IsPlaying())
        {
            FootstepAudioComponent->Play();
        }
    }
    else
    {
        if (FootstepAudioComponent->IsPlaying())
        {
            FootstepAudioComponent->Stop();
        }
    }
}

void AMyPlayerCharacter::StopFootstepAudio()
{
    if (FootstepAudioComponent && FootstepAudioComponent->IsPlaying())
    {
        FootstepAudioComponent->Stop();
    }
}
void AMyPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector HorizontalVelocity = GetVelocity();
    HorizontalVelocity.Z = 0.f;

    if (bIsRunning && (!GetCharacterMovement()->IsMovingOnGround() || HorizontalVelocity.SizeSquared() <= 1.f))
    {
        StopRun();
    }

    // Stamina
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
        PlayerHUD->UpdateCarry(ItemsCarried, MaxItemsCarried);
    }

    // Crouch suave
    float TargetHeight = bIsCrouching ? CrouchHeight : StandingHeight;
    float CurrentHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

    float NewHeight = FMath::FInterpTo(
        CurrentCapsuleHeight,
        TargetHeight,
        DeltaTime,
        CrouchSpeed
    );

    GetCapsuleComponent()->SetCapsuleHalfHeight(NewHeight, true);

    // FOV dinámico al correr
    if (Camera)
    {
        float TargetFOV = bIsRunning ? RunFOV : NormalFOV;

        CurrentDashFOVOffset = FMath::FInterpTo(CurrentDashFOVOffset, 0.f, DeltaTime, DashFOVRecoverSpeed);

        float FinalTargetFOV = TargetFOV + CurrentDashFOVOffset;
        float NewFOV = FMath::FInterpTo(Camera->FieldOfView, FinalTargetFOV, DeltaTime, FOVInterpSpeed);

        Camera->SetFieldOfView(NewFOV);
    }

    if (!GetCharacterMovement()->IsMovingOnGround() || bIsDead || bIsSliding)
    {
        StopFootstepAudio();
    }
}
void AMyPlayerCharacter::TryPlayFootstep()
{
    if (!FootstepSound) return;
    if (!GetCharacterMovement()->IsMovingOnGround()) return;

    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentTime < FootstepBlockedUntil) return;

    float StepInterval = bIsRunning ? RunStepInterval : WalkStepInterval;

    if (CurrentTime - LastFootstepTime < StepInterval) return;

    LastFootstepTime = CurrentTime;

    UGameplayStatics::PlaySoundAtLocation(
        this,
        FootstepSound,
        GetActorLocation()
    );
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

    // NUEVO: soltar objeto con R
    PlayerInputComponent->BindAction("Drop", IE_Pressed, this, &AMyPlayerCharacter::DropItem);
}
void AMyPlayerCharacter::MoveForward(float Value)
{
    UpdateFootstepAudio(Value);

    if (Controller && Value != 0.0f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDir, Value);

        float Loudness = bIsRunning ? 0.8f : 0.4f;
        MakeMovementNoise(Loudness);
    }
}
void AMyPlayerCharacter::DropItem()
{
    // lógica para soltar objeto
}
void AMyPlayerCharacter::MoveRight(float Value)
{
    if (Controller && Value != 0.0f)
    {
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

        const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
        AddMovementInput(RightDir, Value);

        float Loudness = bIsRunning ? 0.8f : 0.4f;
        MakeMovementNoise(Loudness);
    }
}

void AMyPlayerCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void AMyPlayerCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AMyPlayerCharacter::MakeMovementNoise(float Loudness)
{
    UAISense_Hearing::ReportNoiseEvent(
        GetWorld(),
        GetActorLocation(),
        Loudness,
        this
    );
}

void AMyPlayerCharacter::StartJump()
{
    if (!GetCharacterMovement()->IsMovingOnGround()) return;

    Jump();

    MakeMovementNoise(1.0f);

    float CurrentTime = GetWorld()->GetTimeSeconds();
    FootstepBlockedUntil = CurrentTime + FootstepBlockAfterJump;
    LastFootstepTime = CurrentTime;

    if (JumpSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            JumpSound,
            GetActorLocation()
        );
    }
}

void AMyPlayerCharacter::StopJump()
{
    StopJumping();
}

void AMyPlayerCharacter::StartRun()
{
    if (bIsSliding) return;
    if (bIsCrouching) return;
    if (CurrentStamina <= 0.f) return;
    if (!GetCharacterMovement()->IsMovingOnGround()) return;

    FVector HorizontalVelocity = GetVelocity();
    HorizontalVelocity.Z = 0.f;

    if (HorizontalVelocity.SizeSquared() <= 1.f) return;

    bIsRunning = true;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::StopRun()
{
    bIsRunning = false;
    UpdateMovementSpeed();
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
    StopRun();
}

void AMyPlayerCharacter::StopCrouch()
{
    bIsCrouching = false;
    UpdateMovementSpeed();
}

void AMyPlayerCharacter::StartSlide()
{
    if (bIsSliding) return;

    bIsSliding = true;
    bIsCrouching = true;
    bIsRunning = false;

    OriginalGroundFriction = GetCharacterMovement()->GroundFriction;
    GetCharacterMovement()->GroundFriction = SlideFriction;

    FVector SlideDirection = GetVelocity();
    SlideDirection.Z = 0.f;
    SlideDirection = SlideDirection.GetSafeNormal();

    LaunchCharacter(SlideDirection * SlideImpulse, true, false);

    GetWorldTimerManager().SetTimer(
        SlideTimerHandle,
        this,
        &AMyPlayerCharacter::StopSlide,
        SlideDuration,
        false
    );
}

void AMyPlayerCharacter::StopSlide()
{
    bIsSliding = false;
    bIsCrouching = true;

    GetCharacterMovement()->GroundFriction = OriginalGroundFriction;
    UpdateMovementSpeed();
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

    if (DashSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            DashSound,
            GetActorLocation()
        );
    }

    MakeMovementNoise(1.2f);

    GetWorldTimerManager().SetTimer(
        DashCooldownHandle,
        this,
        &AMyPlayerCharacter::ResetDash,
        DashCooldown,
        false
    );
}
void AMyPlayerCharacter::ResetDash()
{
    bCanDash = true;
}

void AMyPlayerCharacter::UpdateMovementSpeed()
{
    float Multiplier = 1.0f - (ItemsCarried * SpeedPenaltyPerItem);
    Multiplier = FMath::Clamp(Multiplier, MinSpeedMultiplier, 1.0f);

    float BaseSpeed = bIsRunning ? RunSpeed : WalkSpeed;

    if (bIsCrouching)
    {
        BaseSpeed *= 0.55f;
    }

    float FinalSpeed = BaseSpeed * Multiplier;
    GetCharacterMovement()->MaxWalkSpeed = FinalSpeed;

    float FinalJump = JumpStrength * Multiplier;
    GetCharacterMovement()->JumpZVelocity = FinalJump;

    UE_LOG(LogTemp, Warning, TEXT("Items: %d | Speed: %f | Jump: %f"),
        ItemsCarried, FinalSpeed, FinalJump);
}

float AMyPlayerCharacter::GetStaminaPercent() const
{
    if (MaxStamina <= 0.f)
    {
        return 0.f;
    }

    return CurrentStamina / MaxStamina;
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

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            2.f,
            FColor::Red,
            TEXT("ESTAS MUERTO")
        );
    }

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
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.f,
                FColor::Red,
                TEXT("NO HAY CHECKPOINT GUARDADO")
            );
        }
        return;
    }

    SetActorLocation(LastCheckpointLocation);

    CurrentHealth = MaxHealth;
    bIsDead = false;

    GetCharacterMovement()->SetMovementMode(MOVE_Walking);
    UpdateMovementSpeed();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        EnableInput(PC);
    }
}