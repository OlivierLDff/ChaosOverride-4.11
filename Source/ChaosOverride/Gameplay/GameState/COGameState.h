// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/GameState.h"
#include "COGameState.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOGameState : public AGameState
{
	GENERATED_BODY()

public:
	class AChaosOverrideGameMode * COGameMode = nullptr;

	//Called on begin play
	virtual void ReceivedGameModeClass() override;
	
	
};
