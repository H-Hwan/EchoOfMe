// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interface/Interactable.h"

#include "PickupItem.generated.h"


class UInventoryItemDefinition;
class UInventoryComponent;


UCLASS()
class ECHOOFME_API APickupItem : public AActor, public IInteractable {
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupItem();

	void Interact_Implementation(AActor* Interactor) override;
	FText GetInteractionPrompt_Implementation() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<UInventoryItemDefinition> ItemDefinition;

private:
	UInventoryComponent* ResolveInventory(AActor* Interactor) const;
};
