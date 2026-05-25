// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EchoPlayerController.generated.h"


class UInventoryComponent;
class URecorderComponent;
class UInputAction;
class UInventoryWidget;


UCLASS()
class ECHOOFME_API AEchoPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEchoPlayerController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void SetupInputComponent() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<URecorderComponent> Recorder;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Interaction")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input|Interaction")
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractDistance = 250.f;

	// 확인용 임시 함수
	void HandleInteract();
};
