// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COCharacterMovementComponent.h"
#include "Character/COCharacter.h"

float UCOCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	const ACOCharacter* CharOwner = Cast<ACOCharacter>(PawnOwner);

	if (CharOwner)
	{/*
		//LOCK SPEED
		if (CharOwner->GetIsLock() && !CharOwner->GetIsCrouching())
		{
			if (CharOwner->GetMoveForwardValue() < 0)
			{
				float CurrentSpeedValue = FMath::Lerp(CharOwner->GetDefaultWalkSpeedValue(), CharOwner->GetLockWalkRightSpeedValue(), FMath::Abs(CharOwner->GetMoveRightValue()));

				MaxSpeed *= FMath::Lerp(CurrentSpeedValue, CharOwner->GetLockWalkBackwardSpeedValue(), FMath::Abs(CharOwner->GetMoveForwardValue()));
			}
			else
			{
				MaxSpeed *= FMath::Lerp(CharOwner->GetDefaultWalkSpeedValue(), CharOwner->GetLockWalkRightSpeedValue(), FMath::Abs(CharOwner->GetMoveRightValue()));
			}
		}



		//CROUCH
		if (CharOwner->GetIsCrouching())
		{
			MaxSpeed = 0.0f;
		}

		if (bPrintChMvtDebugInfo) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("MaxSpeed : %f"), MaxSpeed));*/
		
		//HOVERBOOTS SPEED
		if (CharOwner->GetAreHoverbootsActive() && !IsFalling())
		{
			MaxSpeed *= CharOwner->bHoverbootGraduable ? FMath::Lerp(CharOwner->GetMinMultiplierHoverbootsCoeff(), CharOwner->GetMaxMultiplierHoverbootsCoeff(), CharOwner->GetHoverbootsInputValue()) : CharOwner->GetMaxMultiplierHoverbootsCoeff();
		}
	}
	return MaxSpeed;
}

void UCOCharacterMovementComponent::UpdateComponentVelocity()
{
	Super::UpdateComponentVelocity();

	const ACOCharacter* CharOwner = Cast<ACOCharacter>(PawnOwner);

	if (CharOwner)
	{
		//DASH
		if (CharOwner->GetIsDashing())
		{
			Velocity = CharOwner->GetDashDirection()*CharOwner->GetDashSpeed();
		}

		//DAMAGES
		/*if (CharOwner->GetIsTakingDamages())
		{
			Velocity = CharOwner->GetDamagesVector();
			//Velocity = FVector(0,0,2000.0f)
		}*/

		//GLIDE
		if (CharOwner->GetIsGliding())
		{
			FVector GlideVector = CharOwner->GetGlideVector();
			FVector VelocityProjectOnToGlideVector = Velocity.ProjectOnTo(GlideVector);
			Velocity = Velocity - VelocityProjectOnToGlideVector + GlideVector;
			//Velocity = Velocity + FVector(0, 0, 200);
		}
		//JETPACK
		if (CharOwner->GetIsJetPackActive())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Activate Jet Pack")));
			//Velocity = FMath::Clamp(Velocity+FVector(0,0,CharOwner->GetJetPackZVelocity()), Velocity, )
			MovementMode = EMovementMode::MOVE_Falling;
			Velocity = FVector(Velocity.X, Velocity.Y, FMath::Lerp(CharOwner->GetJetPackMinZVelocity(), CharOwner->GetJetPackMaxZVelocity(), CharOwner->GetJetPackInput()));
		}
	}

}

void UCOCharacterMovementComponent::SetAIIsRunning(bool bIsRunning)
{
	MaxWalkSpeed = bIsRunning ? AIMaxRunningSpeed : AIMaxWalkingSpeed;
}


