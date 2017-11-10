// Fill out your copyright notice in the Description page of Project Settings.

#include "PressurePlateDoorOpener.h"
#include "Engine/World.h"
#include "Engine/StaticMeshActor.h"
#include "Components/PrimitiveComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPressurePlateDoorOpener::UPressurePlateDoorOpener()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPressurePlateDoorOpener::BeginPlay()
{
	Super::BeginPlay();

	start = GetOwner()->GetActorLocation();

	if (!door)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: No Door attached to PressurePlateDoorOpener!"), *GetOwner()->GetName());
	}
	else
	{
		startAngle = door->GetActorRotation().Yaw;
	}

	if (!triggerVolume)
		UE_LOG(LogTemp, Error, TEXT("%s: No TriggerVolume attached to PressurePlateDoorOpener!"), *GetOwner()->GetName());
	
}


// Called every frame
void UPressurePlateDoorOpener::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CloseDoorIfTimeElapsed();

	FVector currentLocation = GetOwner()->GetActorLocation();
	
	float currDistance     = abs(FVector::Dist(currentLocation, start));
	float weightPercentage = GetTotalWeightInTriggerVolume() / weightNeeded;
	///Ignore weight over max
	if (weightPercentage > 1.f) weightPercentage = 1.f;

	float targetDistance    = distance * weightPercentage;
	float remainingDistance = abs(currDistance - targetDistance);

	///Have we reached the distance determined by the current weight?
	if (remainingDistance <= DeltaTime * speed)
	{
		if (weightPercentage >= 0.99f)
		{
			OpenDoor();
		}
		return;
	}	

	int axisDir = (targetDistance > currDistance ? -1 : 1);

	switch (axis)
	{
	case EAxis::Type::X:
		currentLocation.X += DeltaTime * speed * axisDir;
		break;
	case EAxis::Type::Y:
		currentLocation.Y += DeltaTime * speed * axisDir;
		break;
	case EAxis::Type::Z:
		currentLocation.Z += DeltaTime * speed * axisDir;
		break;
	}

	GetOwner()->SetActorLocation(currentLocation);
}

void UPressurePlateDoorOpener::OpenDoor()
{
	lastDoorOpenTime = GetWorld()->GetTimeSeconds();

	if (!isDoorOpen)
	{
		isDoorOpen = true;

		if (door)
			door->SetActorRotation(FRotator(0.f, startAngle + angle, 0.f));

		if (OpenDoorSound)
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				OpenDoorSound,
				1.f,
				1.f,
				0.f
			);
	}
}

void UPressurePlateDoorOpener::CloseDoorIfTimeElapsed()
{

	if (isDoorOpen && (GetWorld()->GetTimeSeconds() - lastDoorOpenTime) > closeDelay)
	{
		isDoorOpen = false;

		if (door)
			door->SetActorRotation(FRotator(0.f, startAngle, 0.f));
		
		if(CloseDoorSound)
			UGameplayStatics::PlaySound2D(
				GetWorld(),
				CloseDoorSound,
				1.f,
				1.f,
				0.f
			);
	}
}

float UPressurePlateDoorOpener::GetTotalWeightInTriggerVolume()
{
	if (!triggerVolume)	return 0;

	TArray<AActor*> actorsOnPressurePlate;
	triggerVolume->GetOverlappingActors(actorsOnPressurePlate);
	
	float totalMass = 0.f;
	for (const AActor* actor : actorsOnPressurePlate)
	{
		totalMass += actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return totalMass;
}

