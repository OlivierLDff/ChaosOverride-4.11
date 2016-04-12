// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COLobbyGameState.h"
#include "Gameplay/GameMode/LobbyGameMode.h"

void ACOLobbyGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ACOLobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACOLobbyGameState, MapName);
	DOREPLIFETIME(ACOLobbyGameState, GameModeCurrentlyChoose);
}



void ACOLobbyGameState::ReceivedGameModeClass()
{
	Super::ReceivedGameModeClass();
	ALobbyGameMode * LobbyGameMode = Cast<ALobbyGameMode>(GetDefaultGameMode());
	/*We check if we are associated with a lobby game mode*/
	if (LobbyGameMode)
	{
		SetCurrentlyChooseGameMode(LobbyGameMode->GetDefaultAvailableGameMode());
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: Failed when trying to get the lobby game mode, verify that game mode you are using with this game state is a LobbyGameMode child")));
	}
}

void ACOLobbyGameState::OnRep_GameModeCurrentlyChoose()
{
}

void ACOLobbyGameState::OnRep_MapName()
{
}

void ACOLobbyGameState::SetCurrentlyChooseGameMode(AChaosOverrideGameMode * NewGameMode)
{
	/*We don't want to put a null game mode as current, that would be bad*/
	if (NewGameMode)
	{
		GameModeCurrentlyChoose = NewGameMode;
		MapName = NewGameMode->GetDefaultMap();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: You are trying to assign a non valid game mode to the player state")));
	}
}

AChaosOverrideGameMode * ACOLobbyGameState::GetCurrentlyChooseGameMode() const
{
	return GameModeCurrentlyChoose;
}
