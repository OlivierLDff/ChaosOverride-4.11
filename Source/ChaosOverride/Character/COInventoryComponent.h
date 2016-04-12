// All Right Reserved Olivier Le Doeuff 2015

#pragma once

#include "Components/ActorComponent.h"
#include "COInventoryComponent.generated.h"

//The enum where all the weapon are defined
UENUM(BlueprintType)
enum class ECOWeapon : uint8
{
	W_Unknown		UMETA(DisplayName = "Unknown"),
	W_Hand			UMETA(DisplayName = "Hand"),
	W_MeleeWeapon	UMETA(DisplayName = "MeleeWeapon"),
	W_Weapon		UMETA(DisplayName = "Weapon"),
	W_Gadget		UMETA(DisplayName = "Gadget")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHAOSOVERRIDE_API UCOInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCOInventoryComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	// Replicate variable to client
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/*------------------------*/
	/*------OWNER SECTION-----*/
	/*------------------------*/

	/*The character owner of this inventory*/
	class ACOCharacter * CharacterOwner = nullptr;

	/*------------------------*/
	/*-----WEAPON SECTION-----*/
	/*------------------------*/

	/*What is actually in hand, replicated for cosmetic purpose*/
	UPROPERTY(Replicated)
		ECOWeapon CurrenlyInHandWeapon = ECOWeapon::W_Hand;

	/*Get the all the weapon in the player state when spawning and create them*/
	virtual void CreateDefaultWeapon();

	/*Create a weapon in the inventory*/
	class ACOGenericWeapon * CreateWeapon(FActorSpawnParameters & SpawnParams, TSubclassOf<class ACOGenericWeapon> WeaponToSpawnClass);

	/*Equip the melee weapon*/
	void EquipMeleeWeapon();
	/*Un equip the melee Weapon*/
	void UnEquipMeleeWeapon();

	/*------------------------*/
	/*--MELEEWEAPON SECTION---*/
	/*------------------------*/

	/*The default melee weapon class for this character*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WEAPON)
	class TSubclassOf<class ACOMeleeWeapon> MeleeWeaponClass = nullptr;
	/*The pointer to the melee weapon spawn for this character when he begin play, replicated for anim instance purpose*/
	UPROPERTY(Replicated)
	class ACOMeleeWeapon * MeleeWeapon = nullptr;

	/*------------------------*/
	/*---FIREWEAPON SECTION---*/
	/*------------------------*/

	/*The inventory where all the weapon that are possessed are stored, the weapon are displayed in the order of the weapon inventory*/
	TArray<class ACOWeapon *> WeaponInventory;

	/*The current COWeapon, it is not what is seen(for example the weapon can be equip but the character has a meleeweapon or nothing in hand, replicated for anim instance purpose*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, Category = WEAPON)
	class ACOWeapon * CurrentWeapon;
	/*The previous COWeapon, for quick change*/
	class ACOWeapon * PreviousWeapon;

	/*The time of the game where the next/previous weapon input have been pressed for the last time*/
	float LastTimeWeaponChanged;
	/*The time required before the input next or prev work again*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = INPUT)
		float TimeBetweenTwoChangedWeapon = 0.2f;
	/*The timer that handle the delay of the set next weapon if LastTimeNextWeaponPressed - TimeBetweenNextPrevWeaponPressed>GameTime
	* /!\ The timer is shared between next and previous weapon input /!\
	*/
	FTimerHandle HandleChangeWeaponTimer;

	//INTERNAL

	/*Set the current weapon, bEquipNewCurrentWeapon if you want to equip it immediatly, for example if the current thing in hand is a melee weapon*/
	virtual void SetCurrentWeapon(class ACOWeapon * Weapon, bool bEquipNow = true, bool bPossessedOnNextRespawn = true);
	/*Set the current weapon from index, bEquipNewCurrentWeapon if you want to equip it immediatly, for example if the current thing in hand is a melee weapon*/
	virtual void SetCurrentWeapon(int32 Index, bool bEquipNow = true);
	/*Set next weapon present in inventory*/
	virtual void SetNextWeapon();
	/*Set previous weapon in inventory*/
	virtual void SetPreviousWeapon();
	/*Set previous weapon in hand the new current weapon*/
	virtual void SetPreviousEquipWeapon();
	/*Equip the current weapon or the first valid weapon if no current weapon are found*/
	virtual bool EquipWeapon();
	/*Equip the weapon passed in param, if the weapon isn't stored in the inventory we had with condition that no other instance of the same class weapon exist, overload of equipWeapon*/
	virtual bool EquipWeapon(class ACOWeapon * Weapon);
	/*Equip the weapon that is stored in the index passed in param, overload of equipWeapon*/
	virtual bool EquipWeapon(int32 Index);
	/*UnEquip the current weapon*/
	virtual bool UnEquipCurrentWeapon(bool bRemoveFromInventory = false);
	/*Check if the weapon is in inventory*/
	virtual bool IsInWeaponInventory(TSubclassOf<class ACOWeapon> NewWeaponClass);
	/*Check if the weapon is in inventory*/
	virtual bool IsInWeaponInventory(ACOWeapon * Weapon);
	/*Get weapon in inventory from the class, return null if not present*/
	virtual ACOWeapon * GetWeaponInInventory(TSubclassOf<class ACOWeapon> NewWeaponClass);
	/*Add a weapon in the inventory, also add it in the player state inventory to not loose it when the character will die, return a pointer to the weapon*/
	virtual class ACOWeapon * AddWeaponInInventory(TSubclassOf<class ACOWeapon> NewWeaponClass, bool bPossessedOnNextRespawn = true, bool bEquipNow = true);
	/*Remove the weapon current from the inventory*/
	virtual bool RemoveWeapon();
	/*Remove the weapon from the inventory*/
	virtual bool RemoveWeapon(TSubclassOf<class ACOWeapon> WeaponClassToRemove, bool bUnPossessedOnNextRespawn = true);
	/*Remove the weapon from the inventory*/
	virtual bool RemoveWeapon(ACOWeapon * Weapon, bool bUnPossessedOnNextRespawn = false);
	/*Remove the weapon at index in the inventoryWeapon from the inventory*/
	virtual bool RemoveWeapon(int32 Index, bool bUnPossessedOnNextRespawn = false);
	/*On Destroy weapon, called by removeWeapon after a delay due to the unequip time*/
	virtual void OnDestroyWeapon(ACOWeapon * Weapon, bool bUnPossessedOnNextRespawn = true);
	/*Destroy all the inventory*/
	virtual void DestroyInventory(bool bPossessedOnNextRespawn = true);
	/*Unequip what is currently in hand*/
	virtual bool UnEquipWhatIsInHand();

	//COSMETIC
	/*Return if the character is actually holding a one hand weapon(current weapon && equiped), should be called by the anim instance*/
	bool GetIsHoldingAOneHandWeapon() const;
	/*Return if the character is actually holding two hand weapon(current weapon && equiped), should be called by the anim instance NOT IMPLEMENT YET TODO*/
	bool GetIsHoldingTwoOneHandWeapon() const;
	/*Return if the character is actually holding a two hand weapon(current weapon && equiped), should be called by the anim instance*/
	bool GetIsHoldingATwoHandWeapon() const;
	/*Return if the character is actually holding a melee weapon(can be the effective melee weapon or a ACOWeapon that work as a melee weapon), should be called by the anim instance*/
	bool GetIsHoldingAMeleeWeapon() const;

	//BLUEPRINT GAMEPLAY

	/*	Add a weapon in the inventory, should be called only via blueprint
	*	This function only execute on server
	*	@param NewWeaponClass The Blueprint class of the weapon you want to add in the inventory
	*	@param bPossessedOnNextRespawn If the weapon should be possessed on the next respawn(it add the weapon class in the default inventory of the player state)
	*	@param bEquipNow If the weapon should be equiped now
	*/
	UFUNCTION(BlueprintCallable, Category = WEAPON)
		virtual void AddWeaponInInventoryBlueprint(TSubclassOf<class ACOWeapon> NewWeaponClass, bool bPossessedOnNextRespawn = true, bool bEquipNow = true);
	/*	Remove a weapon from the inventory, should be called only via blueprint
	*	This function only execute on server
	*	@param WeaponClassToRemove The Blueprint class of the weapon you want to remove from the inventory
	*	@param bUnPossessedOnNextRespawn If the weapon should be unpossessed on the next respawn(it remove the weapon class from the default inventory in the player state)
	*/
	UFUNCTION(BlueprintCallable, Category = WEAPON)
		virtual void RemoveWeaponBlueprint(TSubclassOf<class ACOWeapon> WeaponClassToRemove, bool bUnPossessedOnNextRespawn = true);
	/*	Remove a weapon from the inventory, should be called only via blueprint
	*	This function only execute on server
	*	@param Index The index of the class you want to remove
	*	@param bUnPossessedOnNextRespawn If the weapon should be unpossessed on the next respawn(it remove the weapon class from the default inventory in the player state)
	*/
	UFUNCTION(BlueprintCallable, Category = WEAPON)
		virtual void RemoveWeaponAtIndexBlueprint(int32 Index, bool bUnPossessedOnNextRespawn = true);
	/*	Refill the ammo of the weapon passed in parameter.
	*	This function only execute on server
	*	@param	[in]	TSubclassOf<ACOWeapon> WeaponToRefill : The class of the weapon you want to refill
	*	@param	[in]	uint32 ammo : The number of ammo you want to put in this weapon
	*	@return	[out]	bool &bRefillWasEffective return if the weapon was actually refill, ie if it exist in the inventory if it isn't full
	*/
	UFUNCTION(BlueprintCallable, Category = WEAPON)
		virtual bool RefillAmmoBlueprint(TSubclassOf<ACOWeapon> WeaponToRefill, int32 ammo);

	//DEBUG

	/*Print the weapon Inventory, used to debug*/
	UFUNCTION(BlueprintCallable, Category = WEAPON)
	virtual void DebugPrintWeaponInventory() const;
		
	
};
