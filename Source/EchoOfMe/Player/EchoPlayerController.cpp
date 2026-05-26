// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/EchoPlayerController.h"

#include "Interface/Interactable.h"
#include "Component/InventoryComponent.h"
#include "Component/RecorderComponent.h"
#include "UI/InventoryWidget.h"

#include "EnhancedInputComponent.h"
#include "Camera/PlayerCameraManager.h"


AEchoPlayerController::AEchoPlayerController() {
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Recorder = CreateDefaultSubobject<URecorderComponent>(TEXT("Recorder"));
}

void AEchoPlayerController::BeginPlay() {
	Super::BeginPlay();
}

void AEchoPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent)) {
		if (InteractAction) {
			EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AEchoPlayerController::HandleInteract);
		}
	}
}

void AEchoPlayerController::HandleInteract() {
	if (!PlayerCameraManager) return;

	const FVector Start = PlayerCameraManager->GetCameraLocation();
	const FVector End = Start + PlayerCameraManager->GetCameraRotation().Vector() * InteractDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) {
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UInteractable>()) {
			IInteractable::Execute_Interact(HitActor, GetPawn());
		}
	}
}
