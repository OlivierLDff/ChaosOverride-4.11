// All Right Reserved Olivier Le Doeuff 2015
// Class to display information to the player during a ChaosOverrideGameMode

#pragma once

#include "GameFramework/HUD.h"
#include "COHUD.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API ACOHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	/*Games should override these functions to deal with their game specific logic called by the COGameMOde*/
	void HandleMatchIsWaitingToStart();

	/*Called when the state transitions to InProgress called by the COGameMOde*/
	void HandleMatchHasStarted();

	/*Called when the map transitions to WaitingPostMatch called by the COGameMOde*/
	void HandleMatchHasEnded();
	
	
};
