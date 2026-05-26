// Fill out your copyright notice in the Description page of Project Settings.


#include "ResonanceSensorComponent.h"
#include "Enemy/EchoEnemy.h"
#include "Enemy/EchoEnemyBehaviorComponent.h"

// Sets default values for this component's properties
UResonanceSensorComponent::UResonanceSensorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UResonanceSensorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UResonanceSensorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UResonanceSensorComponent::LightSensorActivate()
{



}

void UResonanceSensorComponent::SoundSensorActivate()
{



}

void UResonanceSensorComponent::SuperNaturalPhenomenonSensor()
{




}

