// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EchoEnemyAIController.generated.h"

class AEchoEnemy;

UCLASS()
class ECHOOFME_API AEchoEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:

	AEchoEnemy* OwnerPawn;


public:

	virtual void BeginPlay() override;




	
};
