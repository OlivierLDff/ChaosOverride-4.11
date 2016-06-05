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

	/**
	*	Spawns and initializes the PlayerState for this Controller
	*/
	virtual void InitPlayerState() override;

	/**
	*	The current Target for this controller
	*	When we set this attribute we bind the onDeathFunction of the COCharacter with the OnTargetDeath of this AIcontroller
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
	class ACOCharacter * TargetCOCharacter = nullptr;

	/** Used in the old system by the blueprint, deprecated*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TARGET)
	class AActor * TargetActor = nullptr;

	/*Call when we are attacking a player*/
	UFUNCTION(BlueprintCallable, Category = TARGET)
		void ExecuteAICombatMode(class AActor * Target);

	/*Call to end an attack*/
	UFUNCTION(BlueprintCallable, Category = TARGET)
		void FinishAICombatMode(class AActor * Target = nullptr);

	/**
	*	Called when our target is killed by someone or by us. Called from a delegate
	*	We unbind this function from the COCharacter onDeath function
	*	Then we set to NULL the TargetCOCharacter attribute and check for a new target
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = TARGET)
		void OnTargetDeath();


	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------RESPAWN SYSTEM---------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	*	Request for the COgamemode to spawn a new pawn for the controller
	*/
	UFUNCTION(BlueprintCallable, Category = RESPAWN)
		virtual void RequestRespawn();

	/**
	*	The timer handle responsible for calling handle respawn on the game mode
	*/
	FTimerHandle HandleRespawnTimer;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------PLAYER STATE SYSTEM-------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	*	Called from the controller's pawn when dying someone
	*	It increments the number of death for this controller in his player state and keep a track of the player state that killed him
	*
	*	@param Killer	The controller that kill the pawn controlled by this controller
	*/
	UFUNCTION(BlueprintCallable, Category = STATE)
		virtual void IncrementDeath(AController * Killer);

	/**
	*	Called from the killed controller's pawn when dying
	*	It increments the number of kill for this controller in his player state and keep a track of the player state that was killed
	*
	*	@param KilledController	The controller killed by this controller's pawn
	*/
	UFUNCTION(BlueprintCallable, Category = STATE)
		virtual void IncrementKill(AController * KilledController);

	/**
	*	Called from the killed controller's pawn when dying
	*	It increments the number of assist for this controller in his player state and keep a track of the player state that was killed by assist
	*
	*	@param KilledByAssistController	The controller killed by another controller by that we made some damage before his death
	*/
	UFUNCTION(BlueprintCallable, Category = STATE)
		virtual void IncrementAssist(AController * KilledByAssistController);

	/**
	*	The player state already cast to a COPlayerState
	*	This attribute is set in InitPlayerState just after the default player state attribute is set
	*/
	//UFUNCTION(BlueprintReadOnly, VisibleAnywhere, Category = STATE)
	class ACOPlayerState * COPlayerState = nullptr;

	/**
	*	@return The COPlayerState attribute
	*/
	UFUNCTION(BlueprintCallable, Category = STATE)
	class ACOPlayerState * GetCOPlayerState() const;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------PSEUDO--------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	*	An array that contain all the default name for the player state when it is created
	*	In the InitPlayerState function we set the default pseudo from one of this name in the player state
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = PSEUDO)
		TArray<FString> DefaultPlayerStatePseudo;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------------AI BEHAVIOR---------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/**
	 *	It is an array containing all the actor percepted by the AIPerception Component
	 */
	TArray<class ACOCharacter *> PerceptedCOCharacter;

	/**
	 *	This function return the best COCharacter that is percepted, the algorithm is changing a lot
	 *	-Look at our TargetCOCharacter to see if he his dead or not. If he is we look for the best sight actor
	 *		-If we can see him we end the function and let him as a target and return.
	 *		-Else We set the actor lost
	 *
	 *	-Look for the best sight actor
	 *		-We loop accross every perceptedActor to find if we can see another one and return the first one correct
	 *
	 *	-If we have lost our actor then we chase him
	 *	-Else we check for sound if we are hearing something not too old and we return this actor
	 *
	 *	@param	PerceptedCOCharacter	An array of all percepted COCharacter given by the OnPerceptionUpdated function
	 *
	 *	@return	ACOCharacter	The best percepted COCharacter
	 */
	virtual class ACOCharacter * FindBestPerceptedCOCharacter(TArray<class ACOCharacter *> PerceptedCOCharacter);

	/**
	*	This function return the best COCharacter that is percepted by sight
	*	For now it only return the first sight percepted COController
	*	It is kind of simulating the behaviour of a normal person that will not shoot especially at the closest person but at the first one seen
	*
	*	@param	PerceptedCOCharacter	An array of all percepted COCharacter given by the OnPerceptionUpdated function
	*
	*	@return	ACOCharacter	The best percepted COCharacter by sight
	*/
	virtual class ACOCharacter * FindBestPerceptedCOCharacterBySight(TArray<class ACOCharacter *> PerceptedCOCharacter);

	/**
	*	This function return the closest COCharacter that emitted sound since DefaultSeachTime
	*
	*	@param	PerceptedCOCharacter	An array of all percepted COCharacter given by the OnPerceptionUpdated function
	*
	*	@return	ACOCharacter	The best percepted COCharacter by sound
	*/
	virtual class ACOCharacter * FindBestPerceptedCOCharacterBySound(TArray<class ACOCharacter *> PerceptedCOCharacter);



};
