// Fill out your copyright notice in the Description page of Project Settings.


#include "FSMStateBase.h"
#include "EchoOfMe/Enemy/EchoEnemyBehaviorComponent.h"

// Sets default values for this component's properties
UFSMStateBase::UFSMStateBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UFSMStateBase::BeginPlay()
{
	Super::BeginPlay();
}

void UFSMStateBase::Startreference(ACharacter* Echo, UEchoEnemyBehaviorComponent* BeHavior)
{
	EnemyBrain = BeHavior;



}


