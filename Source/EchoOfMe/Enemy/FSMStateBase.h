// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FSMStateBase.generated.h"

class UEchoEnemyBehaviorComponent;

UCLASS( ClassGroup=(Custom),Abstract)
class ECHOOFME_API UFSMStateBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFSMStateBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<UEchoEnemyBehaviorComponent> EnemyBrain;

	UPROPERTY()
	TObjectPtr<AEchoEnemy> EchoEnemy;

public:
	UFUNCTION(BlueprintCallable, Category = "State")
	virtual void OnStateEnter() {};
	UFUNCTION(BlueprintCallable, Category = "State")
	virtual void OnStateUpdate(float Delta) {};
	UFUNCTION(BlueprintCallable, Category = "State")
	virtual void OnStateExit() {};


	UFUNCTION()
	virtual void Startreference(AEchoEnemy* Echo, UEchoEnemyBehaviorComponent* BeHavior);

		
};
