// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "COCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API UCOCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
		//On modifie la vitesse du perso ici
		virtual float GetMaxSpeed() const override;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Components|CharacterMovement")
		virtual void UpdateComponentVelocity() override;

public:
	UPROPERTY(EditAnywhere, Category = "Debug")
		bool bPrintChMvtDebugInfo = false;

	/*----------------------------*/
	/*----AI MOUVEMENT SECTION----*/
	/*----------------------------*/

	/*The max running when this character is controlled by an AI, maybe will need some modification when hoverboot are going to be controlled by the AI CharacterMovement::MaxSpeed*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AIMaxRunningSpeed = 500.f;
	/*The max walking when this character is controlled by an AI, maybe will need some modification when hoverboot are going to be controlled by the AI CharacterMovement::MaxSpeed*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
		float AIMaxWalkingSpeed = 200.f;
	/*Set the max speed walk for the AI*/
	UFUNCTION(BlueprintCallable, Category = "AI")
		virtual void SetAIIsRunning(bool bIsRunning = true);
	
	
};
