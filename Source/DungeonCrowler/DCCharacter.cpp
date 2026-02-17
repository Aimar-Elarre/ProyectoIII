#include "DCCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"

ADCCharacter::ADCCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ADCCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ADCCharacter::Dash);
    }
}

void ADCCharacter::Dash()
{
    if (!bCanDash) return;

    bCanDash = false;

    FVector Forward = GetActorForwardVector();
    LaunchCharacter(Forward * 1500.f, true, false);

    GetWorldTimerManager().SetTimerForNextTick([this]()
        {
            bCanDash = true;
        });
}
