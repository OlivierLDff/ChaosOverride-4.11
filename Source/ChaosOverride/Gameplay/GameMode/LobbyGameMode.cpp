// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "LobbyGameMode.h"

void ALobbyGameMode::BeginPlay()
{
	
}

AChaosOverrideGameMode * ALobbyGameMode::GetDefaultAvailableGameMode()
{
	for (int i = 0; i < GameModeAvailable.Num(); ++i)
	{
		/*We check if the default subobject is valid then we access to the class to get the default properties*/
		if (GameModeAvailable[i])
		{
			/*We need a cast here because 'return': cannot convert from 'UObject *' to 'AChaosOverrideGameMode *'*/
			AChaosOverrideGameMode* GameMode = Cast<AChaosOverrideGameMode>(GameModeAvailable[i]->GetDefaultObject());
			if (GameMode)
			{
				if (Cast<ALobbyGameMode>(GameMode))
				{
					/*We don't want to return a lobby game mode as a default game mode*/
					break;
				}
				if (GameMode->IsConfigured())
				{
					/*We check if the class we got is valid & configured*/
					return GameMode;
				}
			}
		}	
	}
	/*If no game mode class are valid we return nullptr*/
	return nullptr;
}

AChaosOverrideGameMode * ALobbyGameMode::GetAvailableGameModeByIndex(int32 Index)
{
	/*If the index is searching information inside the array and not outside*/
	if (Index >= 0 && Index < GameModeAvailable.Num())
	{
		/*We check if the Game Mode subclass is valid*/
		if (GameModeAvailable[Index])
		{
			return Cast<AChaosOverrideGameMode>(GameModeAvailable[Index]->GetDefaultObject());
		}
	}
	return nullptr;
}

TArray<FName> ALobbyGameMode::GetAvailableMapFromGameMode(AChaosOverrideGameMode * COGameMode)
{
	/*We check if the game mode is valid*/
	if (COGameMode)
	{
		/*We check that there is map in the game mode*/
		if (!COGameMode->IsConfigured())
		{
			TArray<FName> AvailableMaps;
			for (int i = 0; i < COGameMode->MapAllowed.Num(); ++i)
			{
				if (!COGameMode->MapAllowed[i].IsNone())
				{
					AvailableMaps.Add(COGameMode->MapAllowed[i]);
				}
			}
			return AvailableMaps;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Error: The game mode you are trying to access the default isn't configured")));
		}
	}
	return TArray<FName>();
}

bool ALobbyGameMode::IsConfigured()
{
	bool result = Super::IsConfigured();
	if (result == false)
	{
		/*We don't need to check further more*/
		return result;
	}
	if (GameModeAvailable.Num() == 0)
	{
		/*There is no default game mode so the lobby isn't configure*/
		return false;
	}
	if (GetDefaultAvailableGameMode())
	{
		/*There is a default available game mode so it's ok */
		return true;
	}
	return false;
}
