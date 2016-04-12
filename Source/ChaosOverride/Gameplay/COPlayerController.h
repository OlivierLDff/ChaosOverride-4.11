// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/PlayerController.h"
#include "COPlayerController.generated.h"

#define PROFILE Profile

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOPlayerController : public APlayerController
{
	GENERATED_BODY()

	ACOPlayerController(const class FObjectInitializer& ObjectInitializer);

public:
	/*Called when spawned in the world*/
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// Replicate variable to client
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*Spawns and initializes the PlayerState for this Controller*/
	virtual void InitPlayerState() override;
	/*Called from Destroyed().*/
	virtual void CleanupPlayerState() override;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------SAVE SYSTEM------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

public:
	/* The profile that contain all the required information about the player */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = PROFILE)
	class UProfileSaveGame* Profile = nullptr;

	/*Used to set the profile to the player controller*/
	UFUNCTION(BlueprintCallable, Category = PROFILE)
	bool SetProfileToController(UProfileSaveGame* newProfile);

	/*Save the current profile, before saving get all the information from the player state to put up to date the PC's profile*/
	bool SaveProfile();

	/*The player state already cast to a COPlayerState*/
	class ACOPlayerState * COPlayerState = nullptr;

	/*Load the current profil info into the player state*/
	bool LoadFromProfile();

	virtual void GameHasEnded
		(
	class AActor * EndGameFocus,
		bool bIsWinner
		) override;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------INPUT SYSTEM-----------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
public:
		virtual void MoveForward(float Value);
		virtual void MoveRight(float Value);
		virtual void Turn(float Value);
		virtual void TurnRate(float Value);
		virtual void LookUp(float Value);
		virtual void LookUpRate(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Hoverboots(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void Jetpack(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void JumpOnPressed();
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void JumpOnReleased();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bJumpPressed;
	void SetJumpPressed(bool Value);
	bool GetJumpPressed() const;

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void PunchOnPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void PunchOnReleased();

	virtual void InteractionOnPressed();
	virtual void InteractionOnReleased();

	virtual void WeaponWheelOnPressed();
	virtual void WeaponWheelOnReleased();

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void AimOnPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void AimOnReleased();

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void FireOnPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void FireOnReleased();
	
	virtual void StartOnPressed();
	virtual void StartOnReleased();

	virtual void SelectOnPressed();
	virtual void SelectOnReleased();

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void PrevWeaponOnPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void PrevWeaponOnReleased();

	UFUNCTION(Server, Reliable, WithValidation)
		virtual void NextWeaponOnPressed();
	UFUNCTION(Server, Reliable, WithValidation)
		virtual void NextWeaponOnReleased();

	/*Get the current COCharacter possess, return null if no CO is possess*/
	class ACOCharacter * GetCOCharacter() const;
	class ACOPlayerState * GetCOPlayerState() const;

	bool CanMove() const;

	bool CanLookAround() const;

	/*Return whether or not the player can jump, may be useful in cinematic for example*/
	bool CanJump() const;

	/*------------------------*/
	/*----HOVERBOOTS INPUT----*/
	/*------------------------*/

	/*Indicate if the controller can activate hoverboots, may be useful in different game mode or map without hoverboots*/
	bool CanActivateHoverboots() const;
	/*Return if the character is allowed to Dash*/
	bool CanDash() const;
	/*Useful to know if the hoverboot input is double pressed*/
	bool bHoverbootInputReleaseSinceLastTimePressed = false;
	bool bHoverboootInputPressed = false;
	bool bDashLaunched = false;
	bool bDashAbort = false;

	bool bFirstTimeDashButtonIsPressed = false;
	bool bDashButtonHasBeenReleased = false;

	/*The max time between two input to determine if the hoverboot input have been double pressed*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DASH)
		float TimeElapseBetweenTwoPressedOfHoverbootsInput = 0.2f;
	/*The timer that handle the reinit of the double pressed hoverboot input*/
	FTimerHandle HoverbootsDoublePressedTimer;
	/*Reinit the bool bHoverbootInputReleaseSinceLastTimePressed to false and clear the timer*/
	void ReinitHoverbootDoublePressedTimer();
	/*The timer that handle the abortion of the dash if the first time the input was pressed took too much time to unpressed*/
	FTimerHandle TimerDashAbort;
	void AbortDash();

	/*------------------------*/
	/*------JETPACK INPUT-----*/
	/*------------------------*/

	bool CanUseJetPack() const;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------RESPAWN SYSTEM---------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*Request for the gamemode to spawn a new pawn for the controller*/
	UFUNCTION(BlueprintCallable, Category = RESPAWN)
	virtual void RequestRespawn();
	/*The timer handle responsible for calling handle respawn on the game mode*/
	FTimerHandle HandleRespawnTimer;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------PLAYER STATE SYSTEM-------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	virtual void IncrementDeath(AController * Killer);
	virtual void IncrementKill(AController * KilledController);
	virtual void IncrementAssist(AController * KilledByAssistController);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------------HUD INTERGACE SYSTEM------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	
	/*return if something bad is actually beeing targeted(ie enemy, ...), should be called by the client HUD*/

	/*return if something good is actually beeing targeted(ie team), should be called by the client HUD*/

	/*return the current life, should be called by the client HUD*/

	/*return the max life, should be called by the client HUD*/

	/*return the current weapon canvas, should be called by the client HUD*/

	/*return the current weapon max ammo, should be called by the client HUD*/

	/*return the current weapon current ammo, should be called by the client HUD*/

	/*return the jet pack tank in percent, should be called by the client HUD*/

	/*return the time of the match, should be called by the client HUD*/

	/*return the time of the best score, should be called by the client HUD*/

	/*return the time of the client score, should be called by the client HUD*/




};
