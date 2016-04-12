// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Gameplay/GameState/COGameState.h"
#include "ChaosOverrideGameMode.h"
#include "COLobbyGameState.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOLobbyGameState : public ACOGameState
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*Called when the GameClass property is set (at startup for the server, after the variable has been replicated on clients)*/
	virtual void ReceivedGameModeClass() override;


public:

	/*The game mode that is currently choose*/
	UPROPERTY(ReplicatedUsing = OnRep_GameModeCurrentlyChoose)
		AChaosOverrideGameMode* GameModeCurrentlyChoose;
	UFUNCTION()
		void OnRep_GameModeCurrentlyChoose();

	/*The map name for the game mode that is choose*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GameMode, ReplicatedUsing = OnRep_MapName)
		FName MapName;
	UFUNCTION()
		void OnRep_MapName();

	/*Set the current choosen game mode, put up to date all relative information like the map name choose for this game mode*/
	UFUNCTION(BlueprintCallable, Category = GAMEMODE)
		void SetCurrentlyChooseGameMode(AChaosOverrideGameMode* NewGameMode);

	/*Get the game mode that is currently choose by the user */
	UFUNCTION(BlueprintCallable, Category = GAMEMODE)
		class AChaosOverrideGameMode* GetCurrentlyChooseGameMode() const;
	
	
};
