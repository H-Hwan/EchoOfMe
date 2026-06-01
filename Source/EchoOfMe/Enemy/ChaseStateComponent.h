// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateBase.h"
#include "ChaseStateComponent.generated.h"


UCLASS(ClassGroup = (FSM), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API UChaseStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	float CurrentTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "ChaseTimer")
	float MaxLoseTime = 7.0f;

	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float Delta) override;
	virtual void OnStateExit() override;

};
