// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"

#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	physicsHandleComponent = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!physicsHandleComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s: No Physics Handle Component attached to Grabber!"), *GetOwner()->GetName());
	}
}

void UGrabber::SetupInputComponent()
{
	inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (inputComponent)
	{
		inputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		inputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s: No Input Component attached to Grabber!"), *GetOwner()->GetName());
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (drawDebug)
	{
		DrawDebugLine(GetWorld(), GetReachLineStart(), GetReachLineEnd(), FColor(255, 0, 0), false, 0.f, 0.f, 5.f);
	}

	if (physicsHandleComponent && physicsHandleComponent->GrabbedComponent)
	{
		physicsHandleComponent->SetTargetLocation(GetReachLineEnd());
	}
}

FVector UGrabber::GetReachLineStart()
{
	FVector playerViewPointLocation;
	FRotator rot;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(playerViewPointLocation, rot);

	return playerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector playerViewPointLocation;
	FRotator playerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(playerViewPointLocation, playerViewPointRotation);

	return playerViewPointLocation + playerViewPointRotation.Vector() * grabRange;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	FHitResult hitResult;

	GetWorld()->LineTraceSingleByObjectType(
		hitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);

	return hitResult;
}

void UGrabber::Grab()
{
	FHitResult hit = GetFirstPhysicsBodyInReach();

	if (physicsHandleComponent && hit.GetActor())
	{
		physicsHandleComponent->GrabComponent(
			hit.GetComponent(),
			NAME_None,
			hit.Location,
			true
		);
	}
}

void UGrabber::Release()
{
	if (physicsHandleComponent)
		physicsHandleComponent->ReleaseComponent();
}


