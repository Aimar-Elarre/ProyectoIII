#include "MyPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

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
    CurrentHealth -= DamageAmount;

    if (CurrentHealth <= 0.f)
    {
        CurrentHealth = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("Personaje muerto"));
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
    if (CurrentStamina > 0.f)
    {
        bIsRunning = true;
        GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
    }
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
        DashCooldownHandle,
        [this]()
        {
            bCanDash = true;
        },
        DashCooldown,
        false
    );
}

