// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interface/Interactable.h"

#include "MemoryFragmentActor.generated.h"

UCLASS()
class ECHOOFME_API AMemoryFragmentActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMemoryFragmentActor();

};
