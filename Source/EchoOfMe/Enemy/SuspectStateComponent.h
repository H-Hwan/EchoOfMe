// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FSMStateBase.h"
#include "SuspectStateComponent.generated.h"




UCLASS(ClassGroup = (FSM), meta = (BlueprintSpawnableComponent))
class ECHOOFME_API USuspectStateComponent : public UFSMStateBase
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool bLooking;

	float LookingTime;

	float LookingForwardTime;

	FRotator RotationToTarget;

	UPROPERTY(EditAnywhere, Category = "Suspect | Angle")
	float LookingAngle = 65.0f;

	UPROPERTY(EditAnywhere, Category = "Suspect | RandRangeMax", meta = (ClampMin = "0", ClampMax = "10"))
	int32 SuspectToAmbush = 5;

	int32 CurrentRandomChangeSuspectToAmbush = 0.0f;

	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float Delta) override;
	virtual void OnStateExit() override;
};
