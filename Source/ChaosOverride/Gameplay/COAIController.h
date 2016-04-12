// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "AIController.h"
#include "COAIController.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOAIController : public AAIController
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACOAIController(const FObjectInitializer& ObjectInitializer);

	/*Spawns and initializes the PlayerState for this Controller*/
	virtual void InitPlayerState() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
	class AActor * TargetActor = nullptr;

	/*Call when we are attacking a player*/
	UFUNCTION(BlueprintCallable, Category = TARGET)
		void ExecuteAICombatMode(class AActor * Target);
	/*Call to end an attack*/
	UFUNCTION(BlueprintCallable, Category = TARGET)
		void FinishAICombatMode(class AActor * Target = nullptr);
	UFUNCTION(BlueprintImplementableEvent, Category = TARGET)
		void OnTargetDeath();


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------RESPAWN SYSTEM---------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*Request for the gamemode to spawn a new pawn for the controller*/
	virtual void RequestRespawn();
	/*The timer handle responsible for calling handle respawn on the game mode*/
	FTimerHandle HandleRespawnTimer;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------PLAYER STATE SYSTEM-------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	virtual void IncrementDeath(AController * Killer);
	virtual void IncrementKill(AController * KilledController);
	virtual void IncrementAssist(AController * KilledByAssistController);

	/*The player state already cast to a COPlayerState*/
	class ACOPlayerState * COPlayerState = nullptr;
	class ACOPlayerState * GetCOPlayerState() const;
	
	
	
};
