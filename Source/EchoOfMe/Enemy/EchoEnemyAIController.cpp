// Fill out your copyright notice in the Description page of Project Settings.


#include "EchoEnemyAIController.h"
#include "Enemy/EchoEnemy.h"
#include "NavigationSystem.h"


void AEchoEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<AEchoEnemy>(GetOwner());



}

