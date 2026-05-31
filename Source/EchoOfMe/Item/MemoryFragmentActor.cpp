// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/MemoryFragmentActor.h"

#include "Component/InventoryComponent.h"
#include "Component/MemoryComponent.h"
#include "Data/MemoryFragmentDefinition.h"

#include "GameFramework/Pawn.h"


// Sets default values
AMemoryFragmentActor::AMemoryFragmentActor() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	// 상호작용 라인트레이트에 잡히도록 설정
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
}


void AMemoryFragmentActor::Interact_Implementation(AActor* Interactor) {
	if (!MemoryDefinition) return;

	UInventoryComponent* Inventory = ResolveInventory(Interactor);
	if (!Inventory || !Inventory->AddItem(MemoryDefinition)) return;

	// 회수 연출 트리거
	if (const APawn* Pawn = Cast<APawn>(Interactor)) {
		if (AController* Con = Pawn->GetController()) {
			if (UMemoryComponent* MemoryComp = Con->FindComponentByClass<UMemoryComponent>()) {
				MemoryComp->HandleMemoryCollected(MemoryDefinition);
			}
		}
	}

	Destroy();
}


FText AMemoryFragmentActor::GetInteractionPrompt_Implementation() const {
	return NSLOCTEXT("Memory", "Recall", "들여다보기");
}


UInventoryComponent* AMemoryFragmentActor::ResolveInventory(AActor* Interactor) const {
	if (!Interactor) return nullptr;

	if (const APawn* Pawn = Cast<APawn>(Interactor)) {
		if (AController* Con = Pawn->GetController()) {
			return Con->FindComponentByClass<UInventoryComponent>();
		}
	}

	return Interactor->FindComponentByClass<UInventoryComponent>();
}
