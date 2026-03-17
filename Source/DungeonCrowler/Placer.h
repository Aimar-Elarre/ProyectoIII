#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Placer.generated.h"

/**
 * APlacer: Punto de spawn dinámico para actores en el mapa
 * 
 * Funcionalidad:
 * - Marca un punto específico donde se instanciará un actor
 * - El tipo de actor a spawnear lo decide el GameState (ACrawGS)
 * - Soporta múltiples spawns por Placer (SpawnCount)
 * - Se destruye tras completar los spawns (para evitar clutter en el juego)
 * 
 * Uso:
 * 1. Colocar APlacer en el mapa
 * 2. El GameState (ACrawGS) buscará todos los Placeres al iniciar
 * 3. Para cada Placer, seleccionará aleatoriamente un actor del pool y lo instanciará
 */

UCLASS()
class DUNGEONCROWLER_API APlacer : public AActor
{
    GENERATED_BODY()

public:
    APlacer();

    virtual void BeginPlay() override;

public:
    /**
     * Número de actores a spawnear en este punto
     * Default: 1 (fácilmente ampliable para spawns múltiples)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
    int32 SpawnCount;

private:
    // Componente visual para identificar el Placer en el editor (esfera de diagnóstico)
    class USphereComponent* VisualizationSphere;
};
