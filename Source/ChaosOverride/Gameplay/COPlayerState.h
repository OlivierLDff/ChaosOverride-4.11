// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/PlayerState.h"
#include "Gameplay/ProfileSaveGame.h"
#include "Weapon/COWeapon.h"
#include "COPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------OVERRIDE--------------------------------------------------------------------------------*/
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
		virtual void ClientInitialize(class AController * C) override;

		virtual void CopyProperties(class APlayerState * PlayerState) override;

		// Replicate variable to client
		virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
		/*-------------------------------------------------------------RAW DATA------------------------------------------------------------------------------*/
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

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
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 Experience = 0;

	/*Rank*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 Rank = 0;

	/*Number of kills*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 Kills = 0;

	/*Number of assists*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 Assists = 0;

	/*Number of death*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 Deaths = 0;

	/*Number of game win*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 GameWin = 0;

	/*Number of game loose*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 GameLoose = 0;

	/*Time playing the game*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		FDateTime TimeOfPlay;

	/*Time playing per mode*/

	/*Pseudo*/
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		FString Pseudo;

	/*LookUpRate*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		float BaseLookUpRate = GetClass()->GetDefaultObject<ACOCharacter>()->BaseLookUpRate;

	/*LookUpReverse*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		bool bIsLookUpRateReverse = GetClass()->GetDefaultObject<ACOCharacter>()->bIsLookUpRateReverse;

	/*TurnUpRate*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		float BaseTurnRate = GetClass()->GetDefaultObject<ACOCharacter>()->BaseTurnRate;

	/*TurnUpReverse*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		bool bIsTurnRateReverse = GetClass()->GetDefaultObject<ACOCharacter>()->bIsTurnRateReverse;

	/*------MATCH DATA-----*/

	/*Team ID, only effective if the teams number in the game mode >1 and <=TeamID */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = GAMEPLAY, Replicated)
		int32 TeamID = 1;

	/*Global experience*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 MatchExperience = 0;

	/*Number of kills*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 MatchKills = 0;
	/*An array containing all the player killed during the match, can contains redondance if kills a player more than one time*/
	TArray<APlayerState *> KilledPlayer;

	/*Number of assists*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 MatchAssists = 0;
	/*An array containing all the player assist killed during the match, can contains redondance if kills a player more than one time*/
	TArray<APlayerState *> AssistKilledPlayer;

	/*Number of death*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = GAMEPLAY, Replicated)
		int32 MatchDeaths = 0;
	/*An array containing all the player killed by during the match, can contains redondance if kills a player more than one time*/
	TArray<APlayerState *> KilledByPlayer;
	


		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------SAVE SYSTEM------------------------------------------------------------------------------*/
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
public:
		/*Copy all the information that we need to save in the profile data, should be called by the player controller only*/
		virtual bool SaveInProfile(UProfileSaveGame * Profile);
		/*Load all the information that we need to save from the profile data, should be called by the player controller only*/
		virtual bool LoadFromProfile(UProfileSaveGame * Profile);
	
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------INVENTORY SYSTEM-------------------------------------------------------------------------*/
		/*-----------------------------------------------------------------------------------------------------------------------------------------------------*/

		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Weapon)
		TArray<TSubclassOf<class ACOWeapon>> WeaponInventory;

		/*Return all the weapon that needed to be spawned on begin play*/
		TArray<TSubclassOf<class ACOWeapon>> GetWeaponInInventory();
		/*Add a weapon in inventory, return false if the weapon is already present*/
		bool AddWeaponInInventory(TSubclassOf<class ACOWeapon>newWeaponClass);
		/*Remove a weapon from the inventory, return false if it wasn't sucessful*/
		bool RemoveWeaponFromInventory(TSubclassOf<class ACOWeapon>newWeaponClass);
		/*Remove a weapon from the inventory, return false if it wasn't sucessful*/
		bool RemoveWeaponFromInventory(ACOWeapon * WeaponToRemove);
		/*Remove all the weapon from the inventory*/
		void CleanInventory();
		/*Fill the inventory with all the weapon that are allowed by the game mode, should be called at the begin of a match*/
		void FillInventoryWithDefaultWeapon(const TArray<TSubclassOf<ACOWeapon>> WeaponArray);
	

};
