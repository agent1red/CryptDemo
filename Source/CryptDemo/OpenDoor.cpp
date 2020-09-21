++// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"


// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	OpenAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("OpenAudioComponent"));
    CloseAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CloseAudioComponent"));

}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// Get Yaw of Door for opening
	InitialYaw = GetOwner() -> GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw; 

	FindPressurePlate();

}

// Check to be sure pressure palte is set for door 
void UOpenDoor::FindPressurePlate()
{
	if (!PressurePlate)
		{
			UE_LOG(LogTemp, Error, TEXT("%s has the door component on i, but no pressureplate set!"), *GetOwner() ->GetName());
		}
}



// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

// Door open based on mass of object to pressure plate plus speed of door opening Yaw axis 
	CurrentTime = GetWorld() ->GetTimeSeconds();

	if (TotalMassOfActors() > MassToOpenDoors)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld() ->GetTimeSeconds();
	}
	// if the door has been opened longer than DoorCloseDelay
	else 
	{
		if (CurrentTime - DoorLastOpened > DoorCloseDelay)
		{
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenAngle, DeltaTime, DoorOpenSpeed);
	FRotator DoorRotation = GetOwner() ->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner() -> SetActorRotation(DoorRotation);

	CloseDoorSound = false;
	if (!OpenAudioComponent) {return;}
	if (!OpenDoorSound)
	{
		OpenAudioComponent ->Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentYaw = FMath::FInterpTo(CurrentYaw, InitialYaw, DeltaTime, DoorClosedSpeed);
	FRotator DoorRotation = GetOwner() ->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner() -> SetActorRotation(DoorRotation);

	OpenDoorSound = false;
	if (!CloseAudioComponent) {return;}
	if (!CloseDoorSound)
	{
		CloseAudioComponent ->Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;


	// Find all Overlapping actors 
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate) {return TotalMass;}
	PressurePlate -> GetOverlappingActors(OUT OverlappingActors);

	// Add up their masses. 
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor ->FindComponentByClass<UPrimitiveComponent>() -> GetMass();
	}
	
	return TotalMass;
}
