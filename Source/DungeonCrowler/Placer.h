#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Placer.generated.h"

/**
 * APlacer: Referencia de posición para spawn dinámico
 * 
 * Solo almacena posición y rotación en el mapa.
 * El GameState (ACrawGS) lee la ubicación para spawnear actores.
 */

UCLASS()
class DUNGEONCROWLER_API APlacer : public AActor
{
    GENERATED_BODY()

public:
    APlacer();
};
