// All Right Reserved Olivier Le Doeuff 2015

#include "ChaosOverride.h"
#include "COInventoryComponent.h"
#include "Character/COCharacter.h"
#include "Gameplay/COPlayerState.h"
#include "Weapon/COWeapon.h"
#include "Weapon/COGenericWeapon.h"
#include "Weapon/MeleeWeapon/COMeleeWeapon.h"

// Sets default values for this component's properties
UCOInventoryComponent::UCOInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	//CharacterOwner = nullptr;

	// ...
}


// Called when the game starts
void UCOInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	/*We get the owner and store him in the characterOwner pointer*/
	/*if (GetOwner())
	{
		CharacterOwner = Cast <ACOCharacter>(GetOwner());
		if (!CharacterOwner)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : No character owner for the component %s, bugs will occured"), *GetName()));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error : No owner for the component %s, bugs will occured"), *GetName()));
	}*/
}


// Called every frame
void UCOInventoryComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}


void UCOInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//WEAPON
	DOREPLIFETIME(UCOInventoryComponent, CurrenlyInHandWeapon);
	DOREPLIFETIME(UCOInventoryComponent, CurrentWeapon);
	DOREPLIFETIME(UCOInventoryComponent, MeleeWeapon);
}

void UCOInventoryComponent::SetCurrentWeapon(ACOWeapon * Weapon, bool bEquipNow, bool bPossessedOnNextRespawn)
{
	if (!CharacterOwner) return;
	if (Weapon && Weapon != CurrentWeapon)
	{
		int32 WeaponIndex;
		/*We check if the weapon is in inventory, if not we create it*/
		if (!WeaponInventory.Find(Weapon, WeaponIndex))
		{
			/*We attach the weapon and add it into the inventory*/
			Weapon->SetOwner(CharacterOwner->GetOwner());
			Weapon->Instigator = CharacterOwner->GetInstigator();
			Weapon->AttachRootComponentTo(CharacterOwner->GetMesh(), Weapon->GetWeaponSocketName());
			WeaponInventory.Add(Weapon);
			if (bPossessedOnNextRespawn && CharacterOwner->COPlayerState)
				CharacterOwner->COPlayerState->AddWeaponInInventory(Weapon->GetClass());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning : the weapon %s have just been created for %s but wasn't in the inventory, should not occured"), *Weapon->GetName(), *GetName()));
		}

		/*If we want to equip the weapon now*/
		if (bEquipNow)
		{
			EquipWeapon(Weapon);
		}
		else
		{
			PreviousWeapon = CurrentWeapon;
			CurrentWeapon = Weapon;
		}
	}
}

void UCOInventoryComponent::SetCurrentWeapon(int32 Index, bool bEquipNow)
{
	if (WeaponInventory[Index] != CurrentWeapon)
	{
		/*We check if the new weapon is different from the first one*/
		if (WeaponInventory[Index]) //Check if the weapon in the array is valid
		{
			/*Equip the weapon if asked*/
			if (bEquipNow)
			{
				EquipWeapon(Index);
			}
			/*We store the new previous weapon*/
			if (CurrentWeapon != WeaponInventory[Index])
			{
				PreviousWeapon = CurrentWeapon;
				CurrentWeapon = WeaponInventory[Index];
			}
		}
	}
}

void UCOInventoryComponent::SetNextWeapon()
{
	// DEBUG GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("We set the next weapon for %s"), *GetName()));
	int8 nextWeaponIndex = -1;
	int8 currentSearchIndex = CurrentWeapon ? CurrentWeapon->IndexInInventory + 1 : 0;//We spawn the first non null weapon if the current weapon is null
	if (CurrentWeapon)
	{
		/*We come back at the begining of the array*/
		if (currentSearchIndex >= WeaponInventory.Num())
			currentSearchIndex = 0;
	}

	/*We search weapon only if the weapon inventory have something stored in it*/
	if (WeaponInventory.Num() != 0)
	{
		/*Get the next index avoiding null weapon, we browse the entire array*/
		for (int i = 0; i < WeaponInventory.Num(); ++i)
		{

			if (WeaponInventory[currentSearchIndex])
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("WeaponInventory[%d] : %s"), currentSearchIndex,*WeaponInventory[currentSearchIndex]->GetName()));
				if (WeaponInventory[currentSearchIndex] != CurrentWeapon) //We found the new index, the weapon is valid and different from the current weapon
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("We found the next weapon at index : %d"), currentSearchIndex));
					nextWeaponIndex = currentSearchIndex;
					break;
				}
			}
			++currentSearchIndex;
			if (currentSearchIndex >= WeaponInventory.Num())
				currentSearchIndex = 0;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%d iteration, currentSearchIndex = %d"),i, currentSearchIndex));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("nextWeaponIndex : %d"), nextWeaponIndex));
		/*A next weapon has been found*/
		if (nextWeaponIndex >= 0)
		{
			/*Equip the nextIndex*/
			EquipWeapon(nextWeaponIndex);
		}
	}
	/*Set the last time the next or previous input have been pressed*/
	LastTimeWeaponChanged = GetWorld()->GetTimeSeconds();
}

void UCOInventoryComponent::SetPreviousWeapon()
{
	int8 nextWeaponIndex = -1;
	int8 currentSearchIndex = CurrentWeapon ? CurrentWeapon->IndexInInventory - 1 : 0;//We spawn the first non null weapon if the current weapon is null
																					  //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("currentSearchIndex before test : %d"), currentSearchIndex));
	if (CurrentWeapon)
	{
		/*We come back at the begining of the array*/
		if (currentSearchIndex < 0)
			currentSearchIndex = WeaponInventory.Num() - 1;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("currentSearchIndex after test : %d"), currentSearchIndex));


	/*We search weapon only if the weapon inventory have something stored in it*/
	if (WeaponInventory.Num() != 0)
	{
		/*Get the next index avoiding null weapon, we browse the entire array*/
		for (int i = 0; i < WeaponInventory.Num(); ++i)
		{

			if (WeaponInventory[currentSearchIndex])
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("WeaponInventory[%d] : %s"), currentSearchIndex,*WeaponInventory[currentSearchIndex]->GetName()));
				if (WeaponInventory[currentSearchIndex] != CurrentWeapon) //We found the new index, the weapon is valid and different from the current weapon
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("We found the next weapon at index : %d"), currentSearchIndex));
					nextWeaponIndex = currentSearchIndex;
					break;
				}
			}
			--currentSearchIndex;
			if (currentSearchIndex < 0)
				currentSearchIndex = WeaponInventory.Num() - 1;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%d iteration, currentSearchIndex = %d"),i, currentSearchIndex));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("prevWeaponIndex : %d"), nextWeaponIndex));
		/*A next weapon has been found*/
		if (nextWeaponIndex >= 0)
		{
			/*Equip the nextIndex*/
			EquipWeapon(nextWeaponIndex);
		}
	}
	/*Set the last time the next or previous input have been pressed*/
	LastTimeWeaponChanged = GetWorld()->GetTimeSeconds();
}

void UCOInventoryComponent::SetPreviousEquipWeapon()
{
	EquipWeapon(PreviousWeapon);
}

bool UCOInventoryComponent::EquipWeapon()
{
	/*Equip the current weapon or the first valid weapon if no current weapon are found*/
	if (CurrentWeapon)
	{
		return EquipWeapon(CurrentWeapon);
	}
	/*If we do not have a current weapon, we search the best weapon in the inventory, the first valid we be good*/
	for (int i = 0; i < WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i]) return EquipWeapon(i);
	}
	/*We return false if no weapon have been found*/
	return false;
}

bool UCOInventoryComponent::EquipWeapon(ACOWeapon * Weapon)
{
	/*We check if the weapon we want to equip is valid*/
	if (Weapon)
	{
		/*We check if the weapon is different that the weapon that is currently in hand
		* If the current weapon is the current weapon and if the weapon is equiped we do no want to change anything
		* NoWeaponInHand		CurrentWeaponDifferentThanWeapon		Result
		*		0							0								1
		*		0							1								1
		*		1							0								0	<- The weapon is already in the hand on is set as current weapon, we have nothing to do here.
		*		1							1								1
		*/
		if (!(CurrenlyInHandWeapon != ECOWeapon::W_MeleeWeapon && !(CurrentWeapon != Weapon)))
		{
			UnEquipWhatIsInHand();
			/*We check if the weapon actually need to be set as current weapon, maybe*/
			if (CurrentWeapon != Weapon)
			{
				if (Weapon->IndexInInventory >= 0)
				{
					int32 findIndex;
					WeaponInventory.Find(Weapon, findIndex);
					/*If the weapon is a good place, then everything is good and we equip it*/
					if (Weapon->IndexInInventory == findIndex)
					{
						SetCurrentWeapon(findIndex, false);
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error, the weapon you are trying to equip to %s isn't in the same inventory index"), *GetName()));
						return false;
					}
					PreviousWeapon = CurrentWeapon;
					CurrentWeapon = Weapon;
				}
			}
			/*We need to equip the weapon*/
			CurrenlyInHandWeapon = ECOWeapon::W_Weapon;
			Weapon->EquipWeapon();
			return true;
		}
	}
	return false;
}

bool UCOInventoryComponent::EquipWeapon(int32 Index)
{
	/*The condition to equip a weapon
	* WeaponInventory[Index] have to be valid
	*			A Weapon Is Equip In Hand						The Current Weapon the same than the weapon we want to equip		Result
	*	(CurrenlyInHandWeapon == ECOWeapon::W_Weapon)					(WeaponInventory[Index]==CurrentWeapon)
	*			0																	0													1
	*			0																	1													1
	*			1																	0													1
	*			1																	1													0 <- the weapon is in hand and is already equip, we have nothing to do
	*/
	if (Index >= 0 && Index < WeaponInventory.Num())
	{
		if (WeaponInventory[Index] && !(CurrenlyInHandWeapon == ECOWeapon::W_Weapon && WeaponInventory[Index] == CurrentWeapon))
		{
			/*UnEquip the stuff in hand*/
			UnEquipWhatIsInHand();
			/*We store the new previous weapon*/
			if (CurrentWeapon != WeaponInventory[Index])
			{
				PreviousWeapon = CurrentWeapon;
				CurrentWeapon = WeaponInventory[Index];
			}
			CurrenlyInHandWeapon = ECOWeapon::W_Weapon;
			WeaponInventory[Index]->EquipWeapon();
			return true;
		}
	}
	return false;
}

bool UCOInventoryComponent::UnEquipCurrentWeapon(bool bRemoveFromInventory)
{
	if (CurrentWeapon)
	{
		CurrenlyInHandWeapon = ECOWeapon::W_Unknown;
		CurrentWeapon->UnEquipWeapon();
		if (bRemoveFromInventory)
		{
			RemoveWeapon(CurrentWeapon);
		}
	}
	return false;
}

bool UCOInventoryComponent::IsInWeaponInventory(TSubclassOf<class ACOWeapon> NewWeaponClass)
{
	// Not tested yet
	for (int i = 0; i < WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i])
		{
			if (WeaponInventory[i]->GetClass() == NewWeaponClass)
				return true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: the character %s have a null weapon in his inventory, that should have never occured"), *CharacterOwner->GetName()));
		}
	}
	return false;
}

bool UCOInventoryComponent::IsInWeaponInventory(ACOWeapon * Weapon)
{
	return WeaponInventory.Find(Weapon) != INDEX_NONE;
}

ACOWeapon * UCOInventoryComponent::GetWeaponInInventory(TSubclassOf<class ACOWeapon> NewWeaponClass)
{
	for (int i = 0; i < WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i])
		{
			if (WeaponInventory[i]->GetClass() == NewWeaponClass)
				return WeaponInventory[i];
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: the character %s have a null weapon in his inventory, that should have never occured"), *CharacterOwner->GetName()));
		}
	}
	return nullptr;
}

void UCOInventoryComponent::CreateDefaultWeapon()
{
	if (!CharacterOwner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error the component %s has no owner so we can't create default weapon"), *GetName()));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = CharacterOwner->GetInstigator();
	SpawnParams.Owner = CharacterOwner->GetOwner();
	//SpawnParams.SpawnCollisionHandlingOverride(ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	/*Create Melee Weapon*/
	MeleeWeapon = Cast<ACOMeleeWeapon>(CreateWeapon(SpawnParams, MeleeWeaponClass));
	if (!MeleeWeapon)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error, please add a default melee weapon to the character : %s"), *CharacterOwner->GetClass()->GetDefaultObjectName().ToString()));

	/*Create COWeapon*/
	if (CharacterOwner->COPlayerState)
	{
		TArray<TSubclassOf<ACOWeapon>> WeaponArray = CharacterOwner->COPlayerState->GetWeaponInInventory();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Default weapon array size : %d for %s"), WeaponArray.Num(), *CharacterOwner->COPlayerState->GetName()));
		for (int i = 0; i < WeaponArray.Num(); ++i)
		{
			AddWeaponInInventory(WeaponArray[i], true, true);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error, can't add the default weapon to the pawn that just spawned because the player state is invalid for %s"), *CharacterOwner->GetName()));
	}
}

ACOGenericWeapon * UCOInventoryComponent::CreateWeapon(FActorSpawnParameters & SpawnParams, TSubclassOf<class ACOGenericWeapon> WeaponToSpawnClass)
{
	if (!CharacterOwner) return nullptr;

	ACOGenericWeapon * Weapon = nullptr;
	if (WeaponToSpawnClass)
	{
		Weapon = GetWorld()->SpawnActor<ACOGenericWeapon>(WeaponToSpawnClass, FVector(0, 0, 0), FRotator(0, 0, 0), SpawnParams);//On spawn l'arme aux coordonnees 0 parce que c'est un offset relatif

		if (Weapon)
		{
			Weapon->SetOwner(CharacterOwner);
			Weapon->Instigator = CharacterOwner;
			if (Weapon->GetWeaponSocketName() == FName())//¨To help when creating new bp weapon, if name == none
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error, The weapon %s you just spawned doesn't have a socket point, please add it in the blueprint class"), *Weapon->GetName()));
			}
			else
			{
				Weapon->AttachRootComponentTo(CharacterOwner->GetMesh(), Weapon->GetWeaponSocketName());
			}
			Weapon->SetActorHiddenInGame(true);
			Weapon->SetActorEnableCollision(false);
			Weapon->SetActorTickEnabled(false);
		}
	}
	return Weapon;
}

ACOWeapon * UCOInventoryComponent::AddWeaponInInventory(TSubclassOf<class ACOWeapon> NewWeaponClass, bool bPossessedOnNextRespawn, bool bEquipNow)
{
	if (!CharacterOwner) return nullptr;
	if (NewWeaponClass)
	{
		/*We check if the weapon is already in inventory*/
		if (!IsInWeaponInventory(NewWeaponClass))
		{
			/*We create the weapon and spawn it*/
			FActorSpawnParameters SpawnParams;
			SpawnParams.Instigator = CharacterOwner->GetInstigator();
			SpawnParams.Owner = CharacterOwner;

			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("The instigator for the weapon we are about to spawn is %s"), SpawnParams.Instigator? *SpawnParams.Instigator->GetName() : *FString("Nll")));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("The Owner for the weapon we are about to spawn is %s"), SpawnParams.Owner ? *SpawnParams.Owner->GetName() : *FString("Nll")));

			/*Spawn the weapon*/
			ACOWeapon * Weapon = Cast<ACOWeapon>(CreateWeapon(SpawnParams, NewWeaponClass));

			if (Weapon)
			{
				/*We add it to the player state for next respawn and in the pawn*/
				int Index = WeaponInventory.Add(Weapon);
				Weapon->SetIndexInInventory(Index);	//We set the weapon index
													/*We add it into the player state*/
				if (bPossessedOnNextRespawn && CharacterOwner->COPlayerState)
					CharacterOwner->COPlayerState->AddWeaponInInventory(NewWeaponClass);
				/*We equip the weapon if we need to*/
				if (bEquipNow)
				{
					SetCurrentWeapon(Weapon);
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: Fail to spawn a %s for %s"), *NewWeaponClass->GetName(), *GetName()));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error: The weapon %s you are trying to add to %s is already in his inventory"), *NewWeaponClass->GetName(), *GetName()));
		}
	}
	return nullptr;
}

bool UCOInventoryComponent::RemoveWeapon()
{
	return RemoveWeapon(0);
}

bool UCOInventoryComponent::RemoveWeapon(TSubclassOf<class ACOWeapon> WeaponClassToRemove, bool bUnPossessedOnNextRespawn)
{
	int32 Index = INDEX_NONE;
	/*We search the index of the corresponding weapon*/
	for (int i = 0; i < WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i])
		{
			if (WeaponInventory[i]->GetClass() == WeaponClassToRemove)
			{
				Index = i;
				break;
			}
		}
	}
	/*Remove the weapon at the index*/
	if (Index == INDEX_NONE)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning : We haven't find the index of the weapon you want to remove(remove weapon subclass override)")));
		return false;
	}
	else
	{
		return RemoveWeapon(Index, bUnPossessedOnNextRespawn);
	}
}

bool UCOInventoryComponent::RemoveWeapon(ACOWeapon * Weapon, bool bUnPossessedOnNextRespawn)
{
	if (Weapon)
	{
		return RemoveWeapon(Weapon->IndexInInventory, bUnPossessedOnNextRespawn);
	}
	return false;
}

bool UCOInventoryComponent::RemoveWeapon(int32 Index, bool bUnPossessedOnNextRespawn)
{
	if (!CharacterOwner) return false;
	/*Test if the index is correct*/
	if (Index >= 0 && Index < WeaponInventory.Num())
	{
		/*Do we need to unequip the weapon*/
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("CurrenlyInHandWeapon %d"), CurrenlyInHandWeapon == ECOWeapon::W_Unknown));
		if (CurrenlyInHandWeapon == ECOWeapon::W_Weapon &&  CurrentWeapon == WeaponInventory[Index] && !CharacterOwner->bPendingDying && !CharacterOwner->bIsDead)
		{
			UnEquipCurrentWeapon();
			if (WeaponInventory[Index])
			{
				if (WeaponInventory[Index]->UnEquipTime == 0)
				{
					/*If the time for unequip is null we destroy the weapon now*/
					OnDestroyWeapon(WeaponInventory[Index]);
				}
				else
				{
					/*If the time isn't 0 for the unequip time we delayed from the desired time*/
					FTimerHandle HandleUnEquipDestroyTimer;
					FTimerDelegate OnDestroyWeaponDelegate = FTimerDelegate::CreateUObject(this, &UCOInventoryComponent::OnDestroyWeapon, WeaponInventory[Index], bUnPossessedOnNextRespawn);
					CharacterOwner->GetWorldTimerManager().SetTimer(HandleUnEquipDestroyTimer, OnDestroyWeaponDelegate, WeaponInventory[Index]->UnEquipTime, false);
				}
				return true;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning, The weapon you are trying to remove is null in the inventory")));
			}
		}
		else
		{
			/*The weapon is not in hand, we can destroy it now*/
			OnDestroyWeapon(WeaponInventory[Index], bUnPossessedOnNextRespawn);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Warning, The index where you are trying to delete a weapon isn't valid")));
	}
	return false;
}

void UCOInventoryComponent::OnDestroyWeapon(ACOWeapon * Weapon, bool bUnPossessedOnNextRespawn)
{
	if (!CharacterOwner) return;
	if (Weapon)
	{
		/*Delete the weapon from the player state inventory*/
		if (bUnPossessedOnNextRespawn && CharacterOwner->COPlayerState)
		{
			CharacterOwner->COPlayerState->RemoveWeaponFromInventory(Weapon);
		}
		/*Delete the reference from the inventory*/
		WeaponInventory.Remove(Weapon);
		/*DEstroy the actor*/
		if (CurrentWeapon == Weapon) CurrentWeapon = nullptr;
		Weapon->Destroy();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Weapon destroy")));
	}

}

void UCOInventoryComponent::DestroyInventory(bool bPossessedOnNextRespawn)
{
	//UnEquipWhatIsInHand();
	for (int32 i = WeaponInventory.Num() - 1; i >= 0; i--)
	{
		ACOWeapon* Weapon = WeaponInventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon, !bPossessedOnNextRespawn);
		}
	}
}

bool UCOInventoryComponent::UnEquipWhatIsInHand()
{
	switch (CurrenlyInHandWeapon)
	{
	case ECOWeapon::W_Hand:
		break;
	case ECOWeapon::W_MeleeWeapon:
		if (MeleeWeapon)
			UnEquipMeleeWeapon();
		return true;
	case ECOWeapon::W_Weapon:
		if (CurrentWeapon)
			UnEquipCurrentWeapon();
		return true;
	}
	return false;
}

void UCOInventoryComponent::EquipMeleeWeapon()
{
	if (MeleeWeapon)
	{
		/*UnEquip the stuff in hand*/
		switch (CurrenlyInHandWeapon)
		{
		case ECOWeapon::W_Hand:
			break;
		case ECOWeapon::W_MeleeWeapon:
			break;
		case ECOWeapon::W_Weapon:
			if (CurrentWeapon)
				UnEquipCurrentWeapon();
			break;
		}
		MeleeWeapon->EquipWeapon();
		CurrenlyInHandWeapon = ECOWeapon::W_MeleeWeapon;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Error, The melee weapon you are trying to equip is invalid for %s"), *GetName()));
	}

}

void UCOInventoryComponent::UnEquipMeleeWeapon()
{
	MeleeWeapon->UnEquipWeapon(true);
	CurrenlyInHandWeapon = ECOWeapon::W_Unknown;
}

bool UCOInventoryComponent::GetIsHoldingAOneHandWeapon() const
{
	if (CurrentWeapon)
	{
		return (CurrentWeapon->GetIsAOneHandWeapon() && CurrenlyInHandWeapon == ECOWeapon::W_Weapon);
	}
	return false;
}

bool UCOInventoryComponent::GetIsHoldingTwoOneHandWeapon() const
{
	return false;
}

bool UCOInventoryComponent::GetIsHoldingATwoHandWeapon() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetIsATwoHandWeapon() && CurrenlyInHandWeapon == ECOWeapon::W_Weapon;
	}
	return false;
}

bool UCOInventoryComponent::GetIsHoldingAMeleeWeapon() const
{
	bool result = false;
	if (MeleeWeapon)
	{
		result = CurrenlyInHandWeapon == ECOWeapon::W_MeleeWeapon;
	}
	if (CurrentWeapon)
	{
		result = result || (CurrenlyInHandWeapon == ECOWeapon::W_Weapon && CurrentWeapon->GetIsAMeleeWeaponType());
	}
	return result;
}

void UCOInventoryComponent::RemoveWeaponAtIndexBlueprint(int32 Index, bool bUnPossessedOnNextRespawn)
{
	RemoveWeapon(Index, bUnPossessedOnNextRespawn);
}

void UCOInventoryComponent::RemoveWeaponBlueprint(TSubclassOf<class ACOWeapon> WeaponClassToRemove, bool bUnPossessedOnNextRespawn)
{
	RemoveWeapon(WeaponClassToRemove, bUnPossessedOnNextRespawn);
}

void UCOInventoryComponent::AddWeaponInInventoryBlueprint(TSubclassOf<class ACOWeapon> NewWeaponClass, bool bPossessedOnNextRespawn, bool bEquipNow)
{
	AddWeaponInInventory(NewWeaponClass, bPossessedOnNextRespawn, bEquipNow);
}

bool UCOInventoryComponent::RefillAmmoBlueprint(TSubclassOf<ACOWeapon> WeaponToRefill, int32 ammo)
{
	ACOWeapon * Weapon = GetWeaponInInventory(WeaponToRefill);
	if (Weapon)
	{
		return Weapon->RefillLoader(ammo);
	}
	return false;
}


void UCOInventoryComponent::DebugPrintWeaponInventory() const
{
	//DEBUG

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("/-----------------------------\\")));
	for (int i = WeaponInventory.Num() - 1; i >= 0; --i)
	{
		if (WeaponInventory[i])
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d %s"), i, *WeaponInventory[i]->GetName()));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%d null"), i));
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("/--------Weapon Array ---------\\")));

}