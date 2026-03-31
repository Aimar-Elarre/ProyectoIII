#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class DUNGEONCROWLER_API AEnemy : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemy();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Chase")
    AActor* TargetActor;

    UPROPERTY(EditAnywhere, Category = "Chase")
    float KillDistance = 200.f;
};