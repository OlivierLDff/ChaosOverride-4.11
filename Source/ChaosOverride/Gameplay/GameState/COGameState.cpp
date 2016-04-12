// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COGameState.h"
#include "ChaosOverrideGameMode.h"

void ACOGameState::ReceivedGameModeClass()
{
	AGameMode * GameMode = GetDefaultGameMode();

	if (GameMode)
	{
		COGameMode = Cast<AChaosOverrideGameMode>(GameMode);
		if (!COGameMode)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Error : The COPlayerState did not initialize properly because the gameMode is %s that isn't a subclass of a ChaosOverrideGameMode"), *GameMode->GetName()));
		}
	}
}
