// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EchoPlayerCharacter.h"

// Sets default values
AEchoPlayerCharacter::AEchoPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEchoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEchoPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEchoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

