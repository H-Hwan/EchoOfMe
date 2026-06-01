// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupItem.h"

#include "Component/InventoryComponent.h"
#include "Component/RecorderComponent.h"
#include "Data/InventoryItemDefinition.h"
#include "Data/RecorderItemDefinition.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"


// Sets default values
APickupItem::APickupItem() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComp);

	// 상호작용 라인트레이스에 잡히도록 설정
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
}

void APickupItem::Interact_Implementation(AActor* Interactor) {
	if (!ItemDefinition) return;

	UInventoryComponent* InventoryComp = ResolveInventory(Interactor);
	if (InventoryComp && InventoryComp->AddItem(ItemDefinition)) {
		// 회수 연출

		UE_LOG(LogTemp, Log, TEXT("[PickupItem] 아이템 회수"));

		if (URecorderItemDefinition* RecorderDefinition = Cast<URecorderItemDefinition>(ItemDefinition)) {
			if (const APawn* Pawn = Cast<APawn>(Interactor)) {
				if (AController* Controller = Pawn->GetController()) {
					if (URecorderComponent* Recorder = Controller->FindComponentByClass<URecorderComponent>()) {
						Recorder->HandleRecorderCollected(RecorderDefinition);
					}
				}
			}
		}

		Destroy();
	}
}


FText APickupItem::GetInteractionPrompt_Implementation() const {
	return NSLOCTEXT("Pickup", "Investigate", "회수하기");
}


UInventoryComponent* APickupItem::ResolveInventory(AActor* Interactor) const {
	if (!Interactor) return nullptr;
	if (const APawn* Pawn = Cast<APawn>(Interactor)) {
		if (AController* Con = Pawn->GetController()) {
			return Con->FindComponentByClass<UInventoryComponent>();
		}
	}

	return Interactor->FindComponentByClass<UInventoryComponent>();
}
