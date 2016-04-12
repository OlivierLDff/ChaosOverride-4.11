// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "ChaosOverrideGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API AChaosOverrideGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	/*Constructor*/
	AChaosOverrideGameMode(const class FObjectInitializer& ObjectInitializer);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------STATE------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*Games should override these functions to deal with their game specific logic*/
	void HandleMatchIsWaitingToStart() override;

	/*Called when the state transitions to InProgress*/
	void HandleMatchHasStarted() override;

	/*Called when the map transitions to WaitingPostMatch*/
	void HandleMatchHasEnded() override;

	/*Called when the match transitions to Aborted*/
	void HandleMatchAborted() override;

	/*Called when the match transitions to LeavingMap*/
	void HandleLeavingMap() override;

	/*Inform every HUD that a player entered or leaved the game, called from post login
	* \params	[in] bLogin : true: the player logged in, false: the player logged out
	*			[in/out] C : The controller that logged in or out
	*/
	void InformEveryHUDOfALoginOrOut(bool bLogin, AController * C);
	/*Inform Every HUD that a player was killed by another player
	* If the killer == nullptr it a suicide*/
	void InformEveryHUDOfAKill(FDamageEvent const& DamageEvent, AController* Killer, AController* Victim);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------ENTRY------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

public:
	/*Handles all player initialization that is shared between the travel methods (i.e. called from both PostLogin() and HandleSeamlessTravelPlayer())*/
	virtual void GenericPlayerInitialization(AController * C) override;
	/*Called after a successful login. This is the first place it is safe to call replicated functions on the PlayerAController.*/
	virtual void PostLogin(APlayerController * NewPlayer) override;

	/*Initialize the game.*/
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/*
	* Called on server during seamless level transitions to get the list of Actors that should be moved into the new level PlayerControllers, 
	* Role < ROLE_Authority Actors, and any non-Actors that are inside an Actor that is in the list (i.e. Object.Outer == Actor in the list) are 
	* all automatically moved regardless of whether they're included here only dynamic actors in the PersistentLevel may be moved (this includes all actors spawned during gameplay) 
	* this is called for both parts of the transition because actors might change while in the middle (e.g. players might join or leave the game)
	*/
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray< AActor * > & ActorList) override;

	/* Init the game with the option given in params
	* @param : Option : you can get it with the function AGameMode::GrabOption()
	* Supported options :
	* NbLocalPlayers=3
	* NbAIPlayers=2
	* MatchLength=60
	* ScoreNeededToWin = 500
	* Teams = 2
	* IsLocalGame = true -- TODO
	* MinRespawnDelay -- TODO
	* Corpse --TODO
	* This function must be overriden for custom game options
	*/
	virtual void InitGameWithOption(FString const & Option);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------LEAVE----------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*Called when a Controller with a PlayerState leaves the match.*/
	virtual void Logout(AController* Exiting) override;

		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------CONFIGURATION----------------------------------------------------------------------------*/
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

public:

	/*Is this game mode a local game or on server?*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Init)
		bool bIsLocalGame = true;

	/*The list of all the map that can be played in that game mode*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Map)
		TArray<FName> MapAllowed;

	/*The name of the Game Mode displayed in the menu*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
		FString DisplayName = "MyCustomGameMode";
	
	/*Duration of a match in seconds*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
		int32 MatchLength = 600;

	/*Set the match length in seconds*/
	virtual void SetMatchLength(int32 const newMatchLength);

	/*Number of team, <=1 mean FFA*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
		int32 Teams = 2;

	/*Set the number of teams, if == 1 there is no team and it is a free for all, each character is in a team with the tag teamX, for example team1 or team2*/
	virtual void SetTeamNumber(int32 const NbTeams);

	/*The score needed to win the game, can be the number of kill or point, may be override in subgamemode
	* If team number ==1 (free for all) the score needed to win is the best player score's
	* If team number >1 (deathmatch, etc...) the score needed to win is the best team score (store in the game state)
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
		int32 ScoreNeededToWin = 50;

	/*Set the score needed to win*/
	virtual void SetScoreNeededToWin(int32 const Score);
	
	/*Return the default Map for the the game mode, null if there is not map*/
	UFUNCTION(BlueprintCallable, Category = CONFIGURATION)
	FName GetDefaultMap();

	/*Return true if the gamemode is configured*/
	UFUNCTION(BlueprintCallable, Category = CONFIGURATION)
	virtual bool IsConfigured();

	/*The number of local player to spawn
	* @warning : This attribute is here only for debug purpose, in future release it will be replace
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	int32 NbLocalPlayers = 1;

	/*Set the NbLocalPlayers*/
	virtual void SetNbLocalPlayers(int32 const newNbLocalPlayers);

	/*The number of AI players to spawn*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	int32 NbAIPlayers = 3;

	/*Set the NbAIPlayers*/
	virtual void SetNbAIPlayers(int32 const newNbAIPlayers);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------WEAPON-------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*The array containing all the default weapon for a game mode that are given on spawn*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = WEAPON)
		TArray<TSubclassOf<class ACOWeapon>> DefaultGameModeWeapon;
	/*Return the array containing all the default weapon for a game mode that are given on spawn*/
	TArray<TSubclassOf<class ACOWeapon>> GetDefaultGameModeWeapon() const;

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------RESPAWN------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	/*The time between death and respawn in seconds, if time ==0 the time is null, if time<0 we become a spectator*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = RESPAWN)
		float RespawnTime = 3.0f;
	/*Called by the controller to request a respawn when the controller pawn died*/
	UFUNCTION(BlueprintCallable, Category = RESPAWN)
	virtual void HandleRequestRespawn(AController * C);

	/*Actualise the player start spot where he respawn, this function should be called before the respawn
	* The algorithm will need to be rework in future release
	*/
	UFUNCTION(BlueprintCallable, Category = RESPAWN)
	void ActualisePlayerStartSpot(AController * C);

	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
	/*--------------------------------------------------------------EXPERIENCE RULES-----------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

	//SCORE\\

	/*The point you get when you kill someone*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		float KillScore = 100.f;
	/*The point you get when you assist kill someone*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		float AssistScore = 25.f;
	/*The point you get when you are killed by someone*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		float DeathScore = 0.f;

	//EXPERIENCE\\

	/*The experience you get when you kill someone*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 KillExperience = 100;
	/*The experience you get when you assist kill someone*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 AssistExperience = 25;
	/*The experience you get when you are killed by someone*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 DeathExperience = 0;

	//MATCH\\

	/*The experience you get when you win a match*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 WinningMatchExperience = 100;
	/*The experience you get when you loose a match*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 LoosingMatchScore = 100;

	/*The experience you get when you are the first one of your team*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 FirstPlaceExperience = 1000;
	/*The experience you get when you are the second one of your team*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 SecondPlaceExperience = 750;
	/*The experience you get when you are the third one of your team*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 ThirdPlaceExperience = 500;

	/*The experience you get for beeing the best killer in your team*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 BestKillerExperience = 2000;
	/*The experience you get for beeing the worst killer in your team*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 WorstKillerExperience = 100;
	/*The experience you get for beeing the one who died the less*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 LessDeadExperience = 2000;
	/*The experience you get for beeing the one who died the most*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 MostDeadExperience = 100;
	/*The experience you get for beeing the one who assist killed the most*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 MostAssistKillExperience = 100;
	/*The experience you get for beeing the one who assist killed the less*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EXPERIENCE)
		int32 LessAssistKillExperience = 100;

};
