#include "MyThirdPersonCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "Kismet/GameplayStatics.h"

AMyThirdPersonCharacter::AMyThirdPersonCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Capsule/mesh vienen de ACharacter

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true; // cámara rota con el controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	// Suele ser típico en third person:
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
}

void AMyThirdPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Añadir Mapping Context (Enhanced Input)
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (IMC_Default)
				{
					Subsystem->AddMappingContext(IMC_Default, 0);
				}
			}
		}
	}
}

void AMyThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EIC) return;

	// Move
	if (IA_Move)
	{
		EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AMyThirdPersonCharacter::Input_Move);
	}

	// Look (tu BP tiene IA_Look y IA_MouseLook -> ambos llaman Aim)
	if (IA_Look)
	{
		EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AMyThirdPersonCharacter::Input_Look);
	}
	if (IA_MouseLook)
	{
		EIC->BindAction(IA_MouseLook, ETriggerEvent::Triggered, this, &AMyThirdPersonCharacter::Input_Look);
	}

	// Jump (Started -> Jump, Completed -> StopJumping)
	if (IA_Jump)
	{
		EIC->BindAction(IA_Jump, ETriggerEvent::Started, this, &AMyThirdPersonCharacter::Input_Jump_Started);
		EIC->BindAction(IA_Jump, ETriggerEvent::Completed, this, &AMyThirdPersonCharacter::Input_Jump_Completed);
	}

	// Dash (Started en tu BP)
	if (IA_Dash)
	{
		EIC->BindAction(IA_Dash, ETriggerEvent::Started, this, &AMyThirdPersonCharacter::Input_Dash_Started);
	}
}

void AMyThirdPersonCharacter::Input_Move(const FInputActionValue& Value)
{
	// IA_Move normalmente es Vector2D (X,Y)
	const FVector2D MoveAxis = Value.Get<FVector2D>();
	if (!Controller) return;

	const FRotator ControlRot = Controller->GetControlRotation();
	const FRotator YawRot(0.f, ControlRot.Yaw, 0.f);

	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	// En tu BP hay X Axis / Y Axis: lo común es:
	AddMovementInput(Forward, MoveAxis.Y);
	AddMovementInput(Right, MoveAxis.X);
}

void AMyThirdPersonCharacter::Input_Look(const FInputActionValue& Value)
{
	// IA_Look / IA_MouseLook -> Vector2D
	const FVector2D LookAxis = Value.Get<FVector2D>();
	if (!Controller) return;

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}

void AMyThirdPersonCharacter::Input_Jump_Started(const FInputActionValue& /*Value*/)
{
	Jump();
}

void AMyThirdPersonCharacter::Input_Jump_Completed(const FInputActionValue& /*Value*/)
{
	StopJumping();
}

void AMyThirdPersonCharacter::Input_Dash_Started(const FInputActionValue& /*Value*/)
{
	if (!bCanDash) return;

	bCanDash = false;

	// En tu BP: GetVelocity * 2.0 -> LaunchCharacter
	const FVector LaunchVel = GetVelocity() * DashVelocityMultiplier;

	// LaunchCharacter(LaunchVelocity, XYOverride, ZOverride)
	// Si quieres replicar fuerte el “empuje”, normalmente override true/true.
	LaunchCharacter(LaunchVel, true, true);

	// Sonido (en tu BP: Play Sound at Location con GetActorLocation)
	if (DashSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DashSound, GetActorLocation());
	}

	// En tu BP: Delay(2.0) -> CanDash = true
	GetWorldTimerManager().ClearTimer(DashCooldownHandle);
	GetWorldTimerManager().SetTimer(
		DashCooldownHandle,
		this,
		&AMyThirdPersonCharacter::ResetDash,
		DashCooldown,
		false
	);
}

void AMyThirdPersonCharacter::ResetDash()
{
	bCanDash = true;
}