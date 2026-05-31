// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interface/Interactable.h"

#include "MemoryFragmentActor.generated.h"


class UMemoryFragmentDefinition;
class UInventoryComponent;


UCLASS()
class ECHOOFME_API AMemoryFragmentActor : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMemoryFragmentActor();


	//---
	// [IInteractable]
public:
	// 상호작용 실행
	void Interact_Implementation(AActor* Interactor) override;

	// 상호작용 프롬포트에 띄울 텍스트
	FText GetInteractionPrompt_Implementation() const override;


protected:
	UPROPERTY(VisibleAnywhere, Category = "Memory")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
	TObjectPtr<UMemoryFragmentDefinition> MemoryDefinition;


private:
	UInventoryComponent* ResolveInventory(AActor* Interactor) const;
};
