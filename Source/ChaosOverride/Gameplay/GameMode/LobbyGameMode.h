// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "ChaosOverrideGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ALobbyGameMode : public AChaosOverrideGameMode
{
	GENERATED_BODY()

		virtual void BeginPlay() override;

	
	
public:
	/*Array that contain all the gamemode that can be choose, value can be null don't forget to check*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GameModeSupported)
		TArray<TSubclassOf<AChaosOverrideGameMode>> GameModeAvailable;

	/*Return true if the gamemode is configured*/
	UFUNCTION(BlueprintCallable, Category = CONFIGURATION)
		virtual bool IsConfigured() override;

	/*Should not be expose thru blueprint, only for test purpose
	*@return the first game mode available, nullptr if there is not mode configure*/
	UFUNCTION(BlueprintCallable, Category = CONFIGURATION)
	AChaosOverrideGameMode * GetDefaultAvailableGameMode();

	/*@return A reference to the game mode stored at the index in param in, nullptr if error*/
	UFUNCTION(BlueprintCallable, Category = CONFIGURATION)
		AChaosOverrideGameMode * GetAvailableGameModeByIndex(int32 Index);

	/*Return all the available map for a game mode, empty array if the game mode isn't configured*/
	UFUNCTION(BlueprintCallable, Category = CONFIGURATION)
		TArray<FName> GetAvailableMapFromGameMode(AChaosOverrideGameMode * COGameMode);


	
};
