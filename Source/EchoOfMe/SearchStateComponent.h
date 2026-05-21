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

	virtual void OnStateEnter() override;
	virtual void OnStateUpdate(float Delta) override;
	virtual void OnStateExit() override;


};
