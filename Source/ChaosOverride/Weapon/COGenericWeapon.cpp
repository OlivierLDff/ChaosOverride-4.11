// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COGenericWeapon.h"


// Sets default values
ACOGenericWeapon::ACOGenericWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

// Called when the game starts or when spawned
void ACOGenericWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACOGenericWeapon::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void ACOGenericWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACOGenericWeapon, bPendingEquip);
	DOREPLIFETIME(ACOGenericWeapon, bIsEquip);
	DOREPLIFETIME(ACOGenericWeapon, bPendingUnEquip);
}

void ACOGenericWeapon::EquipWeapon()
{
	/*Activate the actor*/
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	/*Equip the actor and delay the moment where it is usable*/
	if (EquipTime == 0)
	{
		OnEquip();
	}
	else
	{
		bPendingEquip = true;
		GetWorldTimerManager().SetTimer(EquipTimerHandle, this, &ACOGenericWeapon::OnEquip, EquipTime, false);
	}
}

void ACOGenericWeapon::OnEquip()
{
	GetWorldTimerManager().ClearTimer(EquipTimerHandle);
	bPendingEquip = false;
	bIsEquip = true;
}

void ACOGenericWeapon::UnEquipWeapon(bool bWithAnimation)
{
	/*DeActivate the actor*/
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	/*Equip the actor and delay the moment where it is usable*/
	if (UnEquipTime == 0)
	{
		OnUnEquip();
	}
	else
	{
		bPendingUnEquip = true;
		GetWorldTimerManager().SetTimer(UnEquipTimerHandle, this, &ACOGenericWeapon::OnUnEquip, UnEquipTime, false);
	}
}

void ACOGenericWeapon::OnUnEquip()
{
	GetWorldTimerManager().ClearTimer(UnEquipTimerHandle);
	SetActorHiddenInGame(true);
	bPendingUnEquip = false;
	bIsEquip = false;
}

FName ACOGenericWeapon::GetWeaponSocketName() const
{
	return FName();
}

