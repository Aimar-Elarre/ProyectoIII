#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfoSign.generated.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UBoxComponent;

UCLASS()
class DUNGEONCROWLER_API AInfoSign : public AActor
{
    GENERATED_BODY()

public:
    AInfoSign();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> Root = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> SignMesh = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UTextRenderComponent> SignTextRender = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> InteractionBox = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sign", meta = (MultiLine = true))
    FText SignText = FText::FromString(TEXT("Texto del cartel"));

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sign")
    float TextWorldSize = 24.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sign")
    FColor TextColor = FColor::White;

    UFUNCTION()
    void OnInteractionBoxBeginOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnInteractionBoxEndOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );

public:
    UFUNCTION(BlueprintCallable, Category = "Sign")
    void UpdateSignText();
};