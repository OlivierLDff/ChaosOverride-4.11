// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Animation/AnimInstance.h"
#include "COAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CHAOSOVERRIDE_API UCOAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UCOAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void NativeInitializeAnimation() override;
	/** Executed when the Animation is updated */
	virtual void NativeUpdateAnimation(float DeltaTimeX) override;


	/**
	*	---------------------------------------------PUBLICS ATTRIBUTES---------------------------------------------
	*/

	UFUNCTION(BlueprintCallable, Category = OWNER)
		class ACOCharacter* GetOwningCharacter();

		class ACOCharacter * COCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* AMJump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* AMDoubleJumpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* AMDoubleJumpLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
		UAnimMontage* AMDoubleJumpBackward;



	/*-----JUMP SECTION-----*/

	/** Actualis� � chaque frame, permet de savoir si le perso est entrain de tomber(velocit� suivant l'axe Z non nul) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Jump")
		bool bIsFalling = false;

	/** Permet de faire le blend entre la pose ou le perso marche pendant le saut et celle ou il ne le fait pas */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
		float JumpSlotBlend;

	/** Permet de savoir si le perso � jouer l'animation de saut, reset � chaque atterissage */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
		bool bHasJump = false;

	/** Permet de savoir si le perso � jouer l'animation de Double saut, reset � chaque atterissage */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
		bool bHasDoubleJump = false;

	/** Le nom de la section JumpStart (par d�fault == JumpStart) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpStart = "JumpStart";

	/** Le nom de la section JumpUpLoop (par d�fault == JumpUpLoop) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpLoopUp = "JumpLoopUp";

	/** Le nom de la section JumpUpToDown (par d�fault == JumpUpToDown) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpUpToDown = "JumpUpToDown";

	/** Le nom de la section JumpDownLoop (par d�fault == JumpDownLoop) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpLoopDown = "JumpLoopDown";

	/** Le nom de la section JumpEnd (par d�fault == JumpEnd) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpEnd = "JumpEnd";

	/** Le nom de la section JumpEnd (par d�fault == JumpEnd) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpFallStart = "JumpFallStart";

	/** Le nom de la section DoubleJump (par d�fault == DoubleJump) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SDoubleJump = "DoubleJump";

	/** Le nom de la section JumpStartDash (par d�fault == JumpStartDash) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpStartDash = "JumpStartDash";

	/** La valeur de la v�locit� � partir de laquelle on joue l'animation up to down, par d�fault 400 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		float JumpUpToDownVelocity = 400.0f;

	/** Permet de v�rifier si on peut checker la velocity up to down */
	bool bDoubleJumpAuthorizeToCheck = true;

	/** Gere le blend du blendspace DoubleJumpLock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		bool bLockDoubleJump = false;

	/** Le temps de jouer l'animation du double saut en mode lock */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		float DoubleJumpLockTime;
	/** Le temps de jouer l'animation du double saut en mode lock */
	FTimerHandle DoubleJumpLockTimer;

	/** A quelle point joue-t-on le jump right */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
		float DoubleJumpRightAlpha;

	/** A quelle point joue-t-on le jump backward */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
		float DoubleJumpBackwardAlpha;

	/** A quelle point joue-t-on le jump left */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jump")
		float DoubleJumpLeftAlpha;


	/*-----GLIDE SECTION-----*/

	/** Indique si le perso est entrain de planer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glide")
		bool bIsGliding = false;

	/*-----MOUVEMENT SECTION-----*/

	/** Permet de driver le blend space de d�placement de base gr�ce � la vitesse */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement")
		float Speed = 0.0f;

	/** La direction dans laquelle se d�place le perso, pour driver les BlendSpace */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement")
		float Direction = 0.0f;

	/** La velocite du character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement")
		FVector Velocity;

	/** La velocite du character en retirant la composante sur l'axe de la gravit� */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mouvement")
		FVector LinearVelocity;

	/*The pitch to apply to the character to have a correct aim offset
*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rotation")
		float CameraPitch;



	/*-----LOCK SECTION-----*/

	/** Permet de savoir si le perso est lock ou pas */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lock")
		bool bIsLock = false;

	/*-----LEDGE SECTION-----*/

	/** Permet de savoir si le perso est accroch� ou pas */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ledge Grab")
		bool bIsLedgeGrabbing = false;

	/** Le nom de la section JumpStartLedgeGrab (par d�fault == SJumpStartLedgeGrab) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpStartLedgeGrab;

	/** Le nom de la section JumpEndLedgeGrab (par d�fault == SJumpEndLedgeGrab) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump")
		FName SJumpEndLedgeGrab;

	/** Indique si on s'accrochait la frame d'avant */
	bool bWasLedgeGrabInPreviousFrame;

	/*-----DASH SECTION-----*/

	/** Indique si le perso est dans un dash, permet d'activer le blendSpace ou le h�ros dash(hoverboots active) et d'allumer les hoverboots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
		bool bIsDashing;

	/*-----HOVERBOOTS SECTION-----*/

	/** Indique si le perso est dans un dash, permet d'activer le blendSpace ou le h�ros dash(hoverboots active) et d'allumer les hoverboots */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hoverboots")
		bool bAreHoverbootsActive = false;

	/*-----JETPACK SECTION-----*/

	/*If the jetpack is currently Active*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "JetPack")
		bool bIsJetPackActive = false;

	/*-----WEAPON SECTION-----*/

	/* Permet de savoir si l'on peut tirer*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bCanShoot = true;

	/* Permet de savoir si l'on est entrain de tirer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsShooting = false;

	/** Permet de savoir si l'on doit jouer l'animation du Aim */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsAiming = false;

	/** Permet de savoir si l'on est entrain de viser */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsTargeting = false;

	/** L'animation � jouer quand on tire avec un devastator */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		UAnimMontage* DevastatorShootMontage;

	/*If the character is holding a one handed weapon like the devastator*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsHoldingAOneHandWeapon = false;
	/*If the character is holding two one handed weapon like two devastator*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsHoldingTowOneHandWeapon = false;
	/*If the character is holding a two handed weapon like the lance roquette*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsHoldingATwoHandWeapon = false;
	/*If the character is holding a two handed weapon like the lance roquette*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bIsHoldingAMeleeWeapon = false;

	/*-----DEVASTATOR SECTION-----*/

	/* Indique si le devastator est dans les mains du personnage */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Devastator")
	//	bool bIsDevastatorEquip = false;

	/**
	*	---------------------------------------------PUBLICS FUNCTION---------------------------------------------
	*/

	/** Feed each frame info from the owning pawn */
	UFUNCTION()
		void FeedEachFrameInfo();

	/** Permet de g�r� toute la partie animation du saut */
	UFUNCTION()
		void HandleJump();
	/** Fais jouer l'animation up to down en haut du saut si le perso est en haut et qu'il ne double saute pas */
	UFUNCTION(BlueprintCallable, Category = "Jump")
		void HandleCheckForVelocityUp();

	/** Renvoi true si on se d�croche */
	UFUNCTION(BlueprintCallable, Category = "Ledge Grab")
		bool LedgeGrabValueChanged();

	/** Renvoi true si on se d�croche */
	UFUNCTION(BlueprintCallable, Category = "Shoot")
		void PlayShootAnimation();
	
};
