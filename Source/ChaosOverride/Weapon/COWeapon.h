// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Weapon/COGenericWeapon.h"
#include"COWeapon.generated.h"

//The enum where all the weapon are defined
UENUM(BlueprintType)
namespace ECOWeaponType
{
	enum uint8
	{
		WT_OneHanded,	//UMETA(DisplayName = "One Handed"),
		WT_TwoHanded,	//UMETA(DisplayName = "Two Handed"),
		WT_Melee,		//UMETA(DisplayName = "Melee")
	};
}

UCLASS()
class CHAOSOVERRIDE_API ACOWeapon : public ACOGenericWeapon
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACOWeapon(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/*------------------------*/
	/*---OWNER SECTION----*/
	/*------------------------*/

	/*The owning COCharacter, actualiase on begin play*/
	class ACOCharacter * COCharacter = nullptr;

	/*------------------------*/
	/*---COMPONENT SECTION----*/
	/*------------------------*/

	/** Le root de l'objet */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Root")
		class USceneComponent* Root;

	/* Le mesh de l'arme */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Skeletal Mesh")
		class USkeletalMeshComponent* WeaponMesh;

	/* Le mesh de l'arme */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Spawn")
		class UArrowComponent* MuzzleFlashPoint;

	/*---------------------------*/
	/*-----ANIMATION SECTION-----*/
	/*---------------------------*/

	/*The animation to play when we equip the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset * EquipAnimation = nullptr;
	/*The animation to play when we unequip the weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimationAsset * UnEquipAnimation = nullptr;

	/*------------------------*/
	/*-----EQUIP SECTION------*/
	/*------------------------*/

	/*Equip the weapon to the character*/
	virtual void EquipWeapon() override;
	/*Called when the weapon is equip after a timer*/
	virtual void OnEquip() override;

	/*Unequip the weapon*/
	virtual void UnEquipWeapon(bool bWithAnimation = false) override;
	/*Called when the weapon is unequiped after a timer if the animation is played*/
	virtual void OnUnEquip() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = EQUIP)
		FName WeaponSocketName = FName::FName();

	FName GetWeaponSocketName() const override;

	/* L'index ou est l'arme dans l'inventaire */
	int8 IndexInInventory = -1;
	/*Set the index in inventory of the weapon*/
	void SetIndexInInventory(int8 newIndex);

	/*------------------------*/
	/*------FIRE SECTION------*/
	/*------------------------*/

	//FIRE
public:
	/*Press the weapon trigger, it is the entry point if you want to use a weapon*/
	virtual void PressTrigger();
	/*Release the weapon trigger, it is the entry point if you want to use a weapon*/
	virtual void ReleaseTrigger();
protected:
	/*Called when the trigger is pressed, call the on burst if everthing works*/
	virtual void OnTriggerPressed();
	/*Called when the trigger is released*/
	virtual void OnTriggerReleased();
	/*If the weapon trigger is pressed*/
	bool bTriggerPressed;
public:
	/*Return bTriggerPressed*/
	bool GetTriggerPressed() const;

protected:
	/*Called to make the weapon fire*/
	virtual void Burst();
	/*Called when the weapon fire*/
	virtual void OnBurst();
	/*Return true if the weapon can actually fire, for exemple if fully equiped, have ammo, etc...*/
	virtual bool CanFire();
	/*If the weapon is currently firing*/
	bool bIsFiring = false;
public:
	bool GetIsFiring() const;

	/*The last time the weapon fire*/
	float LastTimeWeaponFire = 0.0f;
	/*The timer that handle the call of the onburst event. Used to respect the fire rate*/
	FTimerHandle HandleOnBurstTimer;


	//PROPERTIES
public:
	/*The projectile that the weapon will spawn when the weapon fire*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PROJECTILE)
		TSubclassOf<class ACOWeaponProjectile> ProjectileClass;
	/** Fire Rate in shoot by seconds, if = 0 this mean that the weapon only fire once */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FIRERATE)
		float FireRate = 5.0f;
protected:
	/*The timer that handle the repeat of the burst while the Trigger is pressed if fireRate > 0*/
		FTimerHandle HandleRepeatBurst;

	/*------------------------*/
	/*------AMMO SECTION------*/
	/*------------------------*/

public:
	/*Refill the loader with the ammo count
	* @param ammo The number of ammo you want to add in the loader
	*/
	virtual bool RefillLoader(int32 ammo);
	/*Unfill the loader with the ammo count
	* @param ammo The number of ammo you want to remove from the loader
	*/
	virtual bool UnFillLoader(int32 ammo);
protected:
	/*Called when the loader is full*/
	virtual void OnLoaderFull();
	/*Called when the loader is empty*/
	virtual void OnLoaderEmpty();
	/*Called when the loader just been refilled*/
	virtual void OnLoaderRefill(int32 ammo);
	/*Called when the loader just been unfilled*/
	virtual void OnLoaderUnfill(int32 ammo);
	/*Called when the weapon try to fire but the loader is empty*/
	virtual void OnTryToFireWithEmptyLoader();
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AMMO)
	/*The current ammo of the weapon, may need to be replicate for cosmetic purpose and HUD*/
	int32 CurrentAmmo;

	/*Return CurrentAmmo*/	
	int32 GetCurrentAmmo() const;
	/*The max number of ammo for this weapon, <=0 means infinite ammo*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = AMMO)
		int32 MaxAmmo = 0;
	/*Return if the loader is empty, maybe replicated for cosmetic used*/
	bool IsEmpty() const;

	//BLUEPRINT EXPOSED FUNCTION
public:
	/*Refill the loader of the weapon with the ammount of ammo
	* This function only execute on server
	* @param ammo The amount of ammo you want to put in the weapon
	*/
	UFUNCTION(BlueprintCallable, Category = AMMO)
		virtual void RefillLoaderBlueprint(int32 ammo);

	/*------------------------*/
	/*---PROPERTIES SECTION---*/
	/*------------------------*/

	/* Indique si quand on tire on focus que les pawn ou alors tout les objets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayableCharacter|Focus")
		bool bFocusOnPawnOnly = true;

	//WEAPON TYPE

	/*If the weapon need one hand or two hand or if it is a melee weapon*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WEAPON TYPE")
		TEnumAsByte<ECOWeaponType::uint8> WeaponType = ECOWeaponType::WT_OneHanded;

	/*Return if the weapon is a one handed weapon*/
	bool GetIsAOneHandWeapon() const;
	/*Return if the weapon is a two handed weapon*/
	bool GetIsATwoHandWeapon() const;
	/*Return if the weapon is a two handed weapon*/
	bool GetIsAMeleeWeaponType() const;

	/*------------------------*/
	/*---TARGETING SECTION----*/
	/*------------------------*/

	//FOCUS

	/* Le point de focus de l'arme quand rien n'est détecté */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TARGET)
		float FocusDistance = 2000.0f;

	/*Return the focus point of this gun when the character is aiming and FVector::VectorNull when there is no focus point
	* If an actor is preset in the targetActor array it will return the location of the bestTargetedActor*/
	FVector GetFocusPoint();

	/* The maximum distance where the target can be to be detect by the weapon as a valid target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TARGET)
		float MaxTargetDistance = 2000.0f;

	/*The doubt about the colinearity when targeting*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TARGET)
		float TargetDoubt = 0.95f;

	/*The list of all classes that can be targeted by this weapon, by default ACOCharacter*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TARGET)
		TArray<TSubclassOf<class AActor>> ActorThatCanBeTarget;

	/*The array that contain all of the currently target actor*/
	TArray<class AActor *> TargetActor;

	/*
	*	Get an actor that is targeted
	*	This function may have different behaviour depending on how it is implemented
	*	By default the function return the best targeted actor base on distance and don't remove it from the TargetActor array
	*	Should only be called by the begin play of the projectile spawned
	*	@param : bool bRemoveActorFromTargetActorArrayWhenSpawningAProjectile, If checked the targetActor array will be remove from an actor every time the function GetBestTargetedActor is called
	*	If the weapon is a multi-target weapon please configure your projectile to do so
	*	@return : AActor * BestTargetActor
	*/
	AActor * GetBestTargetedActor(bool bRemoveActorFromTargetActorArrayWhenSpawningAProjectile = false);

	/*Called on every tick to look for targets, must be overriden if necessary, for exemple for multi target system*/
	virtual void CheckForTarget();

	/*Return if the weapon is currently aiming a target*/
	virtual bool GetFindAtLeastOneTarget() const;

	/*Is the character owning the weapon aiming?, if so we check for a target*/
	virtual bool GetIsOwnerAiming() const;

	/*------------------------*/
	/*------DEBUG SECTION-----*/
	/*------------------------*/

	/*DEBUG : Draw CheckForTarget Debug line*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DEBUG)
		bool bDrawCheckForTargetDebugLine = false;

	/*DEBUG : Draw GetFocusPoint Debug line*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = DEBUG)
		bool bDrawGetFocusPointDebugLine = true;





};
