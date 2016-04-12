// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COPlayerState.h"
#include "ChaosOverrideGameMode.h"

void ACOPlayerState::ClientInitialize(class AController * C)
{
	Super::ClientInitialize(C);
}

void ACOPlayerState::CopyProperties(APlayerState * PlayerState)
{
	Super::CopyProperties(PlayerState);

	ACOPlayerState * COPlayerState = Cast<ACOPlayerState>(PlayerState);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("Copy Properties Called"));
	if (COPlayerState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("COPlayerState : %s"), *PlayerState->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("this : %s"), *this->GetName()));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("PSEUDO : %s"), *Pseudo));
		COPlayerState->Experience = Experience;
		COPlayerState->MatchExperience = MatchExperience;

		COPlayerState->Rank = Rank;

		COPlayerState->Kills = Kills;
		COPlayerState->MatchKills = MatchKills;

		COPlayerState->Assists = Assists;
		COPlayerState->MatchAssists = MatchAssists;

		COPlayerState->Deaths = Deaths;
		COPlayerState->MatchDeaths = MatchDeaths;

		COPlayerState->GameWin = GameWin;

		COPlayerState->GameLoose = GameLoose;

		COPlayerState->TimeOfPlay = TimeOfPlay;

		COPlayerState->Pseudo = Pseudo;

		COPlayerState->BaseLookUpRate = BaseLookUpRate;

		COPlayerState->bIsLookUpRateReverse = bIsLookUpRateReverse;

		COPlayerState->BaseTurnRate = BaseTurnRate;

		COPlayerState->bIsTurnRateReverse = bIsTurnRateReverse;

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("new player state PSEUDO : %s"), *COPlayerState->Pseudo));
	}
	PlayerState->Score = Score;
}

void ACOPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACOPlayerState, Experience, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, Kills, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, Assists, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, Deaths, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, GameWin, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, GameLoose, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, TimeOfPlay, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ACOPlayerState, MatchExperience, COND_OwnerOnly);

	DOREPLIFETIME(ACOPlayerState, Rank);
	DOREPLIFETIME(ACOPlayerState, Pseudo);
	DOREPLIFETIME(ACOPlayerState, MatchKills);
	DOREPLIFETIME(ACOPlayerState, MatchAssists);
	DOREPLIFETIME(ACOPlayerState, MatchDeaths);
}

bool ACOPlayerState::SaveInProfile(UProfileSaveGame * Profile)
{
	if (Profile)
	{
		Profile->Experience = Experience;

		Profile->Rank = Rank;

		Profile->Kills = Kills;

		Profile->Assists = Assists;

		Profile->Deaths = Deaths;

		Profile->GameWin = GameWin;

		Profile->GameLoose = GameLoose;

		Profile->TimeOfPlay = TimeOfPlay;
		
		Profile->Pseudo = Pseudo;

		Profile->BaseLookUpRate = BaseLookUpRate;

		Profile->bIsLookUpRateReverse = bIsLookUpRateReverse;

		Profile->BaseTurnRate = BaseTurnRate;

		Profile->bIsTurnRateReverse = bIsTurnRateReverse;
		
		return true;
	}
	
	return false;
}

bool ACOPlayerState::LoadFromProfile(UProfileSaveGame * Profile)
{
	if (Profile)
	{
		Experience = Profile->Experience;

		Rank = Profile->Rank;

		Kills = Profile->Kills;

		Assists = Profile->Assists;

		Deaths = Profile->Deaths;

		GameWin = Profile->GameWin;

		GameLoose = Profile->GameLoose;

		TimeOfPlay = Profile->TimeOfPlay;

		Pseudo = Profile->Pseudo;

		BaseLookUpRate = Profile->BaseLookUpRate;

		bIsLookUpRateReverse = Profile->bIsLookUpRateReverse;

		BaseTurnRate = Profile->BaseTurnRate;

		bIsTurnRateReverse = Profile->bIsTurnRateReverse;

		return true;
	}

	return false;
}

TArray<TSubclassOf<class ACOWeapon>> ACOPlayerState::GetWeaponInInventory()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Default weapon array size in the player state: %d"), WeaponInventory.Num())); test purpose
	return WeaponInventory;
}

bool ACOPlayerState::AddWeaponInInventory(TSubclassOf<class ACOWeapon> newWeaponClass)
{
	return newWeaponClass ? WeaponInventory.AddUnique(newWeaponClass) >= 0 : false;
}

bool ACOPlayerState::RemoveWeaponFromInventory(TSubclassOf<class ACOWeapon> newWeaponClass)
{
	return WeaponInventory.Remove(newWeaponClass) != 0;
}

bool ACOPlayerState::RemoveWeaponFromInventory(ACOWeapon * WeaponToRemove)
{
	if (WeaponToRemove)
	{
		return WeaponInventory.Remove(WeaponToRemove->GetClass()) != 0;
	}
	return false;
}

void ACOPlayerState::CleanInventory()
{
	WeaponInventory.Empty();
}

void ACOPlayerState::FillInventoryWithDefaultWeapon(const TArray<TSubclassOf<ACOWeapon>> WeaponArray)
{
	for (int i = 0; i < WeaponArray.Num(); ++i)
	{
		if(WeaponArray[i])
		AddWeaponInInventory(WeaponArray[i]);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("The weapon array size for %s is %d"), *GetName(), WeaponInventory.Num())); //test
}


