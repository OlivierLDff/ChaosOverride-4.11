// Fill out your copyright notice in the Description page of Project Settings.

#include "ChaosOverride.h"
#include "ChaosOverrideGameMode.h"
#include "Gameplay/COPlayerState.h"
#include "Gameplay/COPlayerController.h"
#include "Gameplay/HUD/COHUD.h"
#include "GameplayElement/COPlayerStart.h"

AChaosOverrideGameMode::AChaosOverrideGameMode(const class FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseSeamlessTravel = true;
}

void AChaosOverrideGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ACOPlayerController * COPlayerController = Cast<ACOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Iterator.GetIndex()));
		if (COPlayerController)
		{
			ACOHUD * COHUD = Cast<ACOHUD>(COPlayerController->GetHUD());
			if (COHUD)
				COHUD->HandleMatchIsWaitingToStart();
			else
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s haven't the right HUD, please set a COHUD"), *GetName()));
		}
	}
}

void AChaosOverrideGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ACOPlayerController * COPlayerController = Cast<ACOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Iterator.GetIndex()));
		if (COPlayerController)
		{
			ACOHUD * COHUD = Cast<ACOHUD>(COPlayerController->GetHUD());
			if (COHUD)
				COHUD->HandleMatchHasStarted();
			else
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s haven't the right HUD, please set a COHUD"), *GetName()));
		}
	}
}

void AChaosOverrideGameMode::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ACOPlayerController * COPlayerController = Cast<ACOPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), Iterator.GetIndex()));
		if (COPlayerController)
		{
			ACOHUD * COHUD = Cast<ACOHUD>(COPlayerController->GetHUD());
			if (COHUD)
				COHUD->HandleMatchHasEnded();
			else
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : %s haven't the right HUD, please set a COHUD"), *GetName()));

			//DO MORE STUFF HERE
			/*Set time dilatation*/
			/*...*/
		}
	}
}

void AChaosOverrideGameMode::HandleMatchAborted()
{
	Super::HandleMatchAborted();
}

void AChaosOverrideGameMode::HandleLeavingMap()
{
	Super::HandleLeavingMap();
}

void AChaosOverrideGameMode::InformEveryHUDOfALoginOrOut(bool bLogin, AController * C)
{
}

void AChaosOverrideGameMode::InformEveryHUDOfAKill(FDamageEvent const & DamageEvent, AController * Killer, AController * Victim)
{
}

void AChaosOverrideGameMode::GenericPlayerInitialization(AController * C)
{
	/*Can be called if the default pawn is already possessed*/

	/*Initialize the weapon in the player state, we do it before the post login because post login seems to call possessed by*/
	if (C)
	{
		if (C->PlayerState)
		{
			ACOPlayerState * COPlayerState = Cast<ACOPlayerState>(C->PlayerState);
			if (COPlayerState)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("The player state found is : %s for %s"), *NewPlayer->PlayerState->GetName(), *NewPlayer->GetName())); //test
				COPlayerState->FillInventoryWithDefaultWeapon(GetDefaultGameModeWeapon());
				if (Teams <= 1) COPlayerState->TeamID = 0;
			}
		}
	}
	Super::GenericPlayerInitialization(C);
}

void AChaosOverrideGameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	InformEveryHUDOfALoginOrOut(true, NewPlayer);
}

void AChaosOverrideGameMode::InitGame(const FString & MapName, const FString & Options, FString & ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Options = %s"), *Options));
	FString ResultString;
	FString Opt = Options;
	while (UGameplayStatics::GrabOption(Opt, ResultString))
	{
		InitGameWithOption(ResultString);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("ResultString = %s"), *ResultString));
	}
	//Do stuff relative to game init with all the options fill. May be overriden in different sub game mode
}

void AChaosOverrideGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Seamless travel called")));
	
	if (GameState)
	{
		for (int i = 0; i < GameState->PlayerArray.Num(); ++i)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%s store in the seamless actor travel list"), *GameState->PlayerArray[i]->GetName()));
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Added :%s to the list of player to save"));
			//ActorList.Add(GameState->PlayerArray[i]);
		}
	}*/
}

void AChaosOverrideGameMode::InitGameWithOption(FString const & Option)
{
	FString Splitter = "=";
	FString OptionName;
	FString OptionParam;
	if (!Option.Split(Splitter, &OptionName, &OptionParam))	return;

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OptionName = %s"), *OptionName));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OptionParam = %s"), *OptionParam));

	if (OptionName.Equals("MatchLength", ESearchCase::IgnoreCase))
	{
		SetMatchLength(FCString::Atoi(*OptionParam));
		return;
	}
	if (OptionName.Equals("NbAIPlayers", ESearchCase::IgnoreCase))
	{
		SetNbAIPlayers(FCString::Atoi(*OptionParam));
		return;
	}
	if (OptionName.Equals("NbLocalPlayers", ESearchCase::IgnoreCase))
	{
		SetNbLocalPlayers(FCString::Atoi(*OptionParam));
		return;
	}
	if (OptionName.Equals("ScoreNeededToWin", ESearchCase::IgnoreCase))
	{
		SetScoreNeededToWin(FCString::Atoi(*OptionParam));
		return;
	}
	if (OptionName.Equals("Teams", ESearchCase::IgnoreCase))
	{
		SetTeamNumber(FCString::Atoi(*OptionParam));
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Unknown Option : %s with parameter %s"), *OptionName, *OptionParam));
}

void AChaosOverrideGameMode::Logout(AController * Exiting)
{
	Super::Logout(Exiting);

	/*Save all the profile information only for local controller*/
	ACOPlayerController * COPlayerController = Cast <ACOPlayerController>(Exiting);
	if (COPlayerController && COPlayerController->IsLocalPlayerController())
	{
		COPlayerController->SaveProfile();
	}
	InformEveryHUDOfALoginOrOut(false, Exiting);
}

void AChaosOverrideGameMode::SetMatchLength(int32 const newMatchLength)
{
	MatchLength = newMatchLength;
}

void AChaosOverrideGameMode::SetTeamNumber(int32 const NbTeams)
{
	Teams = NbTeams;
}

void AChaosOverrideGameMode::SetScoreNeededToWin(int32 const Score)
{
	ScoreNeededToWin = Score;
}

FName AChaosOverrideGameMode::GetDefaultMap()
{
	if (!(MapAllowed.Num() == 0))
	{
		for (int i = 0; i < MapAllowed.Num(); ++i)
		{
			if (!MapAllowed[i].IsNone())
			{
				return MapAllowed[i];
			}
		}
	}
	return FName();
}

bool AChaosOverrideGameMode::IsConfigured()
{
	/*For the moment we only return if the map is correct, maybe more things will come. Maybe add the display name to see if it has been changed*/
	return !GetDefaultMap().IsNone();
}

void AChaosOverrideGameMode::SetNbLocalPlayers(int32 const newNbLocalPlayers)
{
	NbLocalPlayers = newNbLocalPlayers;
}

void AChaosOverrideGameMode::SetNbAIPlayers(int32 const newNbAIPlayers)
{
	NbAIPlayers = newNbAIPlayers;
}

TArray<TSubclassOf<class ACOWeapon>> AChaosOverrideGameMode::GetDefaultGameModeWeapon() const
{
	return DefaultGameModeWeapon;
}

void AChaosOverrideGameMode::HandleRequestRespawn(AController * C)
{
	ActualisePlayerStartSpot(C);
	RestartPlayer(C);
}

void AChaosOverrideGameMode::ActualisePlayerStartSpot(AController * C)
{
	/*The player state for the controller*/
	ACOPlayerState * COPlayerState = nullptr;
	/*The array that contain every player start in the map*/
	TArray<AActor *> PlayerStartArray;
	/*The player start that actually match with what we want, for now just the correct teamID, later it will be base on distance with other enemy, etc...*/
	TArray<ACOPlayerStart *> TeamPlayerStartArray;
	/*A temp pointer to a COPlayerStart used to test it*/
	ACOPlayerStart * TempPlayerStart = nullptr;
	/*The random int to select a start point*/
	int randomInt = 0;

	/*Get the player state and the team, check if everything is valid*/
	if (C && C->PlayerState && GetWorld())
	{
		COPlayerState = Cast<ACOPlayerState>(C->PlayerState);
		if (COPlayerState)
		{
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACOPlayerStart::StaticClass(), PlayerStartArray);

			/*We search for all the player start that might interest us*/
			for (int i = 0; i < PlayerStartArray.Num(); ++i)
			{
				TempPlayerStart = Cast<ACOPlayerStart>(PlayerStartArray[i]);
				/*TODO : implement a better algorithm here*/
				if (TempPlayerStart && ((TempPlayerStart->TeamID == COPlayerState->TeamID) || COPlayerState->TeamID == 0))
				{
					TeamPlayerStartArray.Add(TempPlayerStart);
				}
				TempPlayerStart = nullptr;
			}
			/*We random select a player start*/
			if (randomInt >= 0 && randomInt < PlayerStartArray.Num() && TeamPlayerStartArray.Num() > 0)
			{
				randomInt = FMath::RandRange(0, TeamPlayerStartArray.Num()-1);
				if (TeamPlayerStartArray[randomInt])
					C->StartSpot = TeamPlayerStartArray[randomInt];
			}
			else
			{
				if (PlayerStartArray.Num() > 0)
				{
					/*We take a default spawn point if we do not find anything ok*/
					randomInt = FMath::RandRange(0, PlayerStartArray.Num() - 1);
					if (randomInt>=0 && randomInt < PlayerStartArray.Num() && PlayerStartArray[randomInt])
						C->StartSpot = PlayerStartArray[randomInt];
				}	
			}
		}
	}
}
