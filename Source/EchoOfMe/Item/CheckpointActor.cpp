// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/CheckpointActor.h"

#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

#include "Player/EchoPlayerController.h"


// Sets default values
ACheckpointActor::ACheckpointActor() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


// Called when the game starts or when spawned
void ACheckpointActor::BeginPlay() {
	Super::BeginPlay();
	
}


void ACheckpointActor::OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
	int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep) {

}
