// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointActor.generated.h"


class UBoxComponent;
class UArrowComponent;


UCLASS()
class ECHOOFME_API ACheckpointActor : public AActor {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpointActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
		int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	TObjectPtr<UBoxComponent> TriggerBox;

	// 리스폰 위치·방향의 기준
	UPROPERTY(VisibleAnywhere, Category = "Checkpoint")
	TObjectPtr<UArrowComponent> RespawnPoint;

private:
	// 한 번만 발동하기 위한 플래그
	bool bActivated = false;   
};
