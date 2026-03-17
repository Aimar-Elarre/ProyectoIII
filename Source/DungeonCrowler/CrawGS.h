#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CrawGS.generated.h"

class APlacer;

/**
 * ACrawGS: GameState personalizado para la gestión de spawning dinámico
 * 
 * Funcionalidad:
 * - Busca todos los APlacer en el mapa al iniciar el juego
 * - Para cada Placer, selecciona aleatoriamente un tipo de actor del pool (ActorPoolClasses)
 * - Spawnea el actor en la posición del Placer
 * - Destruye el Placer tras completar el spawn
 * 
 * Configuración:
 * - Los tipos de actores spawneables se configuran vía inspector (ActorPoolClasses)
 * - Agregar clases como APickupItemActor, ATrapDamageActor, etc.
 * 
 * Ejemplo:
 * 1. En World Settings, asignar GameStateClass = ACrawGS
 * 2. En el inspector del GameState, agregar clases al ActorPoolClasses
 * 3. Al jugar, se instanciará un actor por cada Placer
 */

UCLASS()
class DUNGEONCROWLER_API ACrawGS : public AGameState
{
    GENERATED_BODY()

public:
    ACrawGS();

    virtual void BeginPlay() override;

public:
    /**
     * Pool de tipos de actores que pueden ser spawneados
     * Configurar en el inspector del GameState
     * 
     * Ejemplo: {APickupItemActor, ATrapDamageActor, AMovingTrapActor}
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning|Config")
    TArray<TSubclassOf<AActor>> ActorPoolClasses;

private:
    /**
     * Busca todos los APlacer en el mapa y los procesa
     */
    void ProcessAllPlacers();

    /**
     * Selecciona aleatoriamente una clase del pool
     * @return Clase de actor seleccionada aleatoriamente
     */
    TSubclassOf<AActor> GetRandomActorClass() const;

    /**
     * Procesa un Placer individual: spawnea un actor y lo destruye
     * @param Placer Referencia al Placer a procesar
     */
    void ProcessPlacer(APlacer* Placer);

    // Array temporal para mantener referencias a los Placeres encontrados
    UPROPERTY()
    TArray<APlacer*> FoundPlacers;
};
