#include "MyPlayerCharacter.h"
#include "TimerManager.h"

AMyPlayerCharacter::AMyPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1️⃣ Crear SpringArm
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 0.f;   // FPS
    SpringArm->bUsePawnControlRotation = true;

    // 2️⃣ Crear Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = true;

    // 3️⃣ Ajustar posición DESPUÉS de crearla
    Camera->SetRelativeLocation(FVector(0.f, 0.f, 64.f));

    // 4️⃣ Configuración FPS
    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AMyPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    CurrentHealth = MaxHealth;
}

void AMyPlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMyPlayerCharacter::StartJump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMyPlayerCharacter::StopJump);
    PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerCharacter::LookUp);
    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyPlayerCharacter::Dash);
}

void AMyPlayerCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorForwardVector(), Value);
    }
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

void AMyPlayerCharacter::Dash()
{
    if (!bCanDash) return;

    bCanDash = false;

    FVector ForwardDirection = GetActorForwardVector();
    LaunchCharacter(ForwardDirection * DashStrength, true, true);

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

