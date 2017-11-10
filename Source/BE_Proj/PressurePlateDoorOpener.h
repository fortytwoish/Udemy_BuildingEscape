// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "Sound/SoundCue.h"
#include "PressurePlateDoorOpener.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BE_PROJ_API UPressurePlateDoorOpener : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPressurePlateDoorOpener();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CloseDoorIfTimeElapsed();

	void OpenDoor();

private:

	/// ------------------------ Door ---------------

	UPROPERTY(EditAnywhere, Category = Door) AActor* door = nullptr;
	//Time until the door closes when pressure plate no longer triggered
	UPROPERTY(EditAnywhere, Category = Door) float closeDelay = 1.f;
	//Final angle of the door, relative to its start angle (Y axis)
	UPROPERTY(EditAnywhere, Category = Door) float angle = -90.0;
	UPROPERTY(EditAnywhere, Category = Door) USoundCue* OpenDoorSound = nullptr;
	UPROPERTY(EditAnywhere, Category = Door) USoundCue* CloseDoorSound = nullptr;
	float startAngle;
	float lastDoorOpenTime;
	float GetTotalWeightInTriggerVolume();
	bool isDoorOpen = false;

	/// ------------------------ Pressure Plate ------------------------

	UPROPERTY(EditAnywhere, Category = PressurePlate) ATriggerVolume* triggerVolume = nullptr;
	//Axis the pressure plate moves along when triggered
	UPROPERTY(EditAnywhere, Category = PressurePlate) TEnumAsByte<EAxis::Type> axis = EAxis::Type::X;
	UPROPERTY(EditAnywhere, Category = PressurePlate) bool invertAxis = false;
	UPROPERTY(EditAnywhere, Category = PressurePlate) float speed = 100.0;
	//Maximum distance for the pressure plate. Door opens when this distance is reached.
	UPROPERTY(EditAnywhere, Category = PressurePlate) float distance;
	//Minimum weight necessary to push the pressure plate to the bottom
	UPROPERTY(EditAnywhere, Category = PressurePlate) float weightNeeded = 100.f;
	FVector start;	
};
