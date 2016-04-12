// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COAnimInstance.h"
#include "Character/COCharacter.h"

UCOAnimInstance::UCOAnimInstance(const class FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UCOAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UCOAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);
	FeedEachFrameInfo();
}

ACOCharacter * UCOAnimInstance::GetOwningCharacter()
{
	return Cast<ACOCharacter>(TryGetPawnOwner());
}

void UCOAnimInstance::FeedEachFrameInfo()
{
	COCharacter = GetOwningCharacter();
	//SPEED
	if (COCharacter)
	{
		Velocity = COCharacter->GetVelocity();
		LinearVelocity = Velocity - Velocity.ProjectOnTo(COCharacter->GetGravityDirection());
		Speed = (Velocity - Velocity.ProjectOnTo(COCharacter->GetGravityDirection())).Size();

		//LEDGE GRAB
		//bIsLedgeGrabbing = COCharacter->GetIsLedgeGrabbing();

		//GLIDE
		bIsGliding = COCharacter->GetIsGliding(); //&& bCanGlide;


		//LOCK
		//bIsLock = COCharacter->GetIsLock();
		bIsTargeting = COCharacter->GetIsAiming();

		//CALCULATE DIRECTION
		Direction = LinearVelocity.Size() ? CalculateDirection(LinearVelocity, COCharacter->GetActorRotation()) : 0;

		//AIM
		bIsAiming = bIsLock || bIsTargeting;

		//CALCULATE PITCH
		CameraPitch = bIsTargeting ? COCharacter->GetBaseAimRotation().Pitch : 0.0f;

		//HOVERBOOTS
		bAreHoverbootsActive = COCharacter->GetAreHoverbootsActive();

		//DOUBLE JUMP BLEND VALUES
		if (Direction >= -180.0f && Direction < -90.0f)
		{
			DoubleJumpRightAlpha = 0.0f;
			DoubleJumpBackwardAlpha = FMath::Lerp(1.0f, 0.0f, (Direction + 180.0f) / 90.0f);
			DoubleJumpLeftAlpha = FMath::Lerp(0.0f, 1.0f, (Direction + 180.0f) / 90.0f);
		}
		if (Direction >= -90.0f && Direction < 0.0f)
		{
			DoubleJumpRightAlpha = 0.0f;
			DoubleJumpBackwardAlpha = 0.0f;
			DoubleJumpLeftAlpha = FMath::Lerp(1.0f, 0.0f, (Direction + 90.0f) / 90.0f);
		}
		if (Direction >= 0.0f && Direction <90.0f)
		{
			DoubleJumpRightAlpha = FMath::Lerp(0.0f, 1.0f, Direction / 90.0f);
			DoubleJumpBackwardAlpha = 0.0f;
			DoubleJumpLeftAlpha = 0.0f;
		}
		if (Direction >= 90.0f && Direction <= 180.0f)
		{
			DoubleJumpRightAlpha = FMath::Lerp(1.0f, 0.0f, (Direction - 90.0f) / 90.0f);
			DoubleJumpBackwardAlpha = FMath::Lerp(0.0f, 1.0f, (Direction - 90.0f) / 90.0f);
			DoubleJumpLeftAlpha = 0.0f;
		}

		//DASH
		bIsDashing = COCharacter->GetIsDashing();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("bIsDashing = %f"), bIsDashing ? 1.0f : 0.0f));

		//JETPACK
		bIsJetPackActive = COCharacter->GetIsJetPackActive();

		//WEAPON
		//bCanShoot = COCharacter->CanFire();
		//bIsDevastatorEquip = OwningPlayableCharacter->GetWeaponEquip() == EWeaponEnum::VE_Devastator;
		//bIsShooting = COCharacter->GetIsShooting() && (!bHasDoubleJump || bCanGlide);
		bIsHoldingAOneHandWeapon = COCharacter->GetIsHoldingAOneHandWeapon();
		bIsHoldingTowOneHandWeapon = COCharacter->GetIsHoldingTwoOneHandWeapon();
		bIsHoldingATwoHandWeapon = COCharacter->GetIsHoldingATwoHandWeapon();
		bIsHoldingAMeleeWeapon = COCharacter->GetIsHoldingAMeleeWeapon();
		
		//JUMP
		HandleJump();
	}	
	
}

void UCOAnimInstance::HandleJump()
{
	if (COCharacter)
	{
		if (COCharacter->GetCharacterMovement())
		{
			/** Permet de savoir à tout instant si le perso est entrian de tomber */
			bIsFalling = COCharacter->IsFalling();
		}

		/** ------------On gère le blend entre l'animation d'atterissage de course ou statique -------------*/

		if (Montage_IsActive(AMJump))
		{
			/* Si la section courante est jumpEnd alors on met le jump slot blend de façon proportionnel à la vitesse, ainsi l'animation sera fluide en fonction de la vitesse
			* C'est à dire que si le perso court il atterira en courant, si il est statique l'animation d'atterissage se jouera sur tout le corp */
			if (Montage_GetCurrentSection(AMJump) == "JumpEnd")
			{
				//On divise la vitesse par la vitesse max pour avoir un résultat entre 0 et 1 et on a besoin du complément à 1
				if(COCharacter->GetCharacterMovement())
				JumpSlotBlend = FMath::Clamp(1 - (Speed / COCharacter->GetCharacterMovement()->MaxWalkSpeed), 0.0f, 1.0f);
			}
			else
			{
				JumpSlotBlend = 1.0f;
			}
		}

		/** ------------On gere les cas si le perso tombe où non -------------*/

		if (bIsFalling)
		{
			if (Montage_IsActive(AMJump))
			{
				/* Si l'animation de JumpEnd n'est pas fini quand on saute alors on met la prochaine section JumpStart sinon on joue juste le montage */
				if ((Montage_GetCurrentSection() == SJumpEnd) && (COCharacter->GetVelocity().Z >= 0))
				{
					Montage_JumpToSection(SJumpStart, AMJump);
				}
			}
			else
			{
				/*----------------------------PLAY JUMP ANIMATION-------------------------------------*/

				//Si le montage n'est pas actif et que l'on est entrain de tomber
				//On regarde s'il a déjà sauter
				if (!bHasJump)
				{
					/*if (LedgeGrabValueChanged() && COCharacter->GetJumpPressed())
					{
						//On se décroche en sautant
						Montage_Play(AMJump);
						Montage_JumpToSection(SJumpStartLedgeGrab, AMJump);
						Montage_SetNextSection(SJumpStartLedgeGrab, SJumpLoopUp, AMJump);
					}
					else
					{
						//On se décroche en appuyant sur retour
						Montage_Play(AMJump);
						Montage_JumpToSection(SJumpUpToDown, AMJump);

					}*/
					//Si on appuie sur le bouton sauter il faut jouer l'animation depuis le début sinon il faut commencer à JumpFallStart
					if (COCharacter->GetJumpPressed())
					{
						Montage_Play(AMJump);
					}
					else
					{
						Montage_Play(AMJump);
						Montage_JumpToSection(SJumpFallStart, AMJump);
						Montage_SetNextSection(SJumpFallStart, SJumpLoopDown, AMJump);
					}
					bHasJump = true;
				}
			}

			/*----------------------------PLAY DOUBLE JUMP ANIMATION-------------------------------------*/

			if ((COCharacter->GetIsDoubleJumping()) && !bHasDoubleJump && (Montage_GetCurrentSection(AMJump) != SDoubleJump))
			{
				bDoubleJumpAuthorizeToCheck = false;

				FName CurrentSection = Montage_GetCurrentSection(AMJump);

				//Montage_JumpToSectionsEnd(CurrentSection, AMJump);
				//Montage_SetNextSection(CurrentSection, SDoubleJump, AMJump);

				Montage_JumpToSection(SDoubleJump, AMJump);
				Montage_SetNextSection(SDoubleJump, SJumpUpToDown, AMJump);

				if (bIsLock)
				{
					//On lance tout les animMontages
					Montage_Play(AMDoubleJumpLeft);
					Montage_Play(AMDoubleJumpBackward);
					Montage_Play(AMDoubleJumpRight);
				}

				bHasDoubleJump = true;
			}

		}
		else
		{
			/*---------------------FAIT ATTERRIR LE PERSO-------------------------------*/

			//Si le perso touche le sol mais n'est pas entrain de jouer l'animation JumpEnd on lui force la main
			if (Montage_IsActive(AMJump))
			{
				FName CurrentSection = Montage_GetCurrentSection(AMJump);

				//On commence par regarder si on ne s'accroche pas à un rebord
				if (bIsLedgeGrabbing && CurrentSection != SJumpEndLedgeGrab && CurrentSection != SJumpEnd) //
				{
					Montage_JumpToSectionsEnd(CurrentSection, AMJump);
					Montage_SetNextSection(CurrentSection, SJumpEndLedgeGrab, AMJump);

				}

				if (!bIsLedgeGrabbing && CurrentSection != SJumpEnd && CurrentSection != SJumpEndLedgeGrab) //
				{
					Montage_JumpToSectionsEnd(CurrentSection, AMJump);
					Montage_SetNextSection(CurrentSection, SJumpEnd, AMJump);
				}
			}
			/** REINIT DES DO ONCE */

			//On réautorise le perso à sauter dès que le montage saut n'est pas actif
			bHasJump = false;
			bHasDoubleJump = false;
			bDoubleJumpAuthorizeToCheck = true;
		}
	}
}

void UCOAnimInstance::HandleCheckForVelocityUp()
{
	ACOCharacter * COCharacter = GetOwningCharacter();
	//APPELLER DEPUIS LE BLUEPRINT
	if (COCharacter)
	{
		if (bDoubleJumpAuthorizeToCheck && (COCharacter->GetVelocity().Z < JumpUpToDownVelocity))
		{
			Montage_SetNextSection(Montage_GetCurrentSection(AMJump), SJumpUpToDown, AMJump);
		}
	}

}

bool UCOAnimInstance::LedgeGrabValueChanged()
{
	if (bWasLedgeGrabInPreviousFrame != bIsLedgeGrabbing && bWasLedgeGrabInPreviousFrame == true)
	{
		bWasLedgeGrabInPreviousFrame = bIsLedgeGrabbing;
		return true;
	}
	else
	{
		bWasLedgeGrabInPreviousFrame = bIsLedgeGrabbing;
		return false;
	}
}

void UCOAnimInstance::PlayShootAnimation()
{
	if (bIsHoldingAOneHandWeapon && DevastatorShootMontage)
	{
		Montage_Play(DevastatorShootMontage);
	}
}