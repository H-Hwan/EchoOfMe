#include "Item/MemoryFragmentActor.h"

#include "Component/InventoryComponent.h"
#include "Component/MemoryComponent.h"
#include "Component/RecorderComponent.h"
#include "Data/MemoryFragmentDefinition.h"

#include "GameFramework/Pawn.h"


AMemoryFragmentActor::AMemoryFragmentActor() {
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);

	// 상호작용 라인트레이트에 잡히도록 설정
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Block);
}


void AMemoryFragmentActor::Interact_Implementation(AActor* Interactor) {
	if (!MemoryDefinition) return;

	const APawn* Pawn = Cast<APawn>(Interactor);
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	URecorderComponent* Recorder = Controller
		? Controller->FindComponentByClass<URecorderComponent>()
		: nullptr;
	if (!Recorder || !Recorder->IsRecorderCollected()) {
		UE_LOG(LogTemp, Log, TEXT("[Memory] 녹음기 회수 전 - 기억조각 회수 차단"));
		return;
	}

	UInventoryComponent* Inventory = ResolveInventory(Interactor);
	if (!Inventory || !Inventory->AddItem(MemoryDefinition)) return;

	// 회수 연출 트리거
	if (UMemoryComponent* MemoryComp = Controller->FindComponentByClass<UMemoryComponent>()) {
		MemoryComp->HandleMemoryCollected(MemoryDefinition);
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
