// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateBase.h"
#include "SearchStateComponent.generated.h"




UCLASS(ClassGroup = (FSM), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API USearchStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FVector LastLocation = FVector::ZeroVector;

	float CurrentTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Wander Time")
	float WanderHoldTimer = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Wander Time")
	float ChangeStateTimer = 10.0f;

	float StuckTimer = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Search | RandRangeMax", meta = (ClampMin = "0", ClampMax = "10"))
	int32 SearchToAmbush = 2;

	int32 CurrentRandomChangeSearchToAmbush = 0.0f;

	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float Delta) override;
	virtual void OnStateExit() override;


};
