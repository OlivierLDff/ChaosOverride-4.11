// All Right Reserved Olivier Le Doeuff 2015
/*
*	A pickable actor is an actor that can be placed in the world and when a COCharacter approche it trigger a function(must be implement in blueprint)
*	A pickable object is defined by a mesh, a sphere component for now
*/

#include "ChaosOverride.h"
#include "Pickable.h"


// Sets default values
APickable::APickable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickable::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

