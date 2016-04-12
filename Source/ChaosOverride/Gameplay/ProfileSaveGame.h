// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/SaveGame.h"
#include "Character/COCharacter.h"
#include "ProfileSaveGame.generated.h"

#define GAMEPLAY Gameplay

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API UProfileSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

		/*Array of playable character information
		*	-Unlock
		*	-Per character Experience
		*	-Time playing a character
		*	-Skin
		*	-
		*
		*/

		/*
		*	Array of weapon
		*	Time using a weapon
		*	Number of kill
		*	Number of assist
		*	Experience
		*	Skin
		*/

		/*Global experience*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 Experience;

	/*Rank*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 Rank;

	/*Number of kills*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 Kills;

	/*Number of assists*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 Assists;

	/*Number of death*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 Deaths;

	/*Number of game win*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 GameWin;

	/*Number of game loose*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		int32 GameLoose;

	/*Time playing the game*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY)
		FDateTime TimeOfPlay;

	/*Time playing per mode*/

	/*Pseudo*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GAMEPLAY)
		FString Pseudo;

	/*LookUpRate*/
		float BaseLookUpRate = GetClass()->GetDefaultObject<ACOCharacter>()->BaseLookUpRate;

	/*LookUpReverse*/
		bool bIsLookUpRateReverse = GetClass()->GetDefaultObject<ACOCharacter>()->bIsLookUpRateReverse;

	/*TurnUpRate*/
		float BaseTurnRate = GetClass()->GetDefaultObject<ACOCharacter>()->BaseTurnRate;
		
	/*TurnUpReverse*/
		bool bIsTurnRateReverse = GetClass()->GetDefaultObject<ACOCharacter>()->bIsTurnRateReverse;
		
	
};
