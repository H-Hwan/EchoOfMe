// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateBase.h"
#include "PatrolStateComponent.generated.h"



UCLASS(ClassGroup = (FSM), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API UPatrolStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	float StuckTime = 0.0f;

	float MaxStuckTime = 8.0f;


	float Suspectmin = 0.0f;

	float Suspectmax = 6.0f;

	float TargetRetryTimer = 0.0f;


	UPROPERTY(EditAnywhere, Category = "Chase | RandRangeMax", meta = (ClampMin = "0", ClampMax = "10"))
	int32 ChaseToAmbush = 5;

	int32 CurrentRandomChangeChaseToAmbush = 0.0f;

	FVector RandomPickTarget = FVector::ZeroVector;

	bool bHasTarget = false;



	virtual void OnStateEnter() override;

	virtual void OnStateUpdate(float Delta) override;

	virtual void OnStateExit() override;

};
