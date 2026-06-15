#include "Component/MemoryComponent.h"

#include "Data/MemoryFragmentDefinition.h"
#include "EchoGameManager.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


UMemoryComponent::UMemoryComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UMemoryComponent::HandleMemoryCollected(const UMemoryFragmentDefinition* Definition) {
	if (!Definition) return;

	// GameManager에 등록
	if (UEchoGameManager* GM = UEchoGameManager::Get(this)) {
		GM->AddMemoryFlag(Definition->FlagKey);
	}

	UE_LOG(LogTemp, Log, TEXT("[Memory] 회수: %s"), *Definition->ItemID.ToString());

	// 회수음 종료 후 시작할 스토리 대상 보관
	PendingStoryDefinition = const_cast<UMemoryFragmentDefinition*>(Definition);

	// 사운드 재생
	if (Definition->FlashbackSound) {
		// 이전 재생이 남아있다면 정리 후 새로 시작
		if (ActiveFlashback) {
			ActiveFlashback->OnAudioFinished.RemoveAll(this);
			ActiveFlashback->Stop();
			ActiveFlashback = nullptr;
		}

		ActiveFlashback = UGameplayStatics::SpawnSound2D(this, Definition->FlashbackSound);
		if (ActiveFlashback) {
			ActiveFlashback->OnAudioFinished.AddDynamic(this, &UMemoryComponent::HandleFlashbackFinished);
		}
		else {
			// 스폰 실패 시에도 스토리는 진행
			HandleFlashbackFinished();
		}
	}
	else {
		// 회수음이 없는 조각이면 바로 스토리 시작
		HandleFlashbackFinished();
	}
	UE_LOG(LogTemp, Log, TEXT("[Memory] 자막: %s"), *Definition->FlashbackText.ToString());

	OnMemoryCollected.Broadcast(Definition);
}


void UMemoryComponent::HandleFlashbackFinished() {
	UE_LOG(LogTemp, Warning, TEXT("[진단] 회수음 종료 콜백 진입"));
	if (ActiveFlashback) {
		ActiveFlashback->OnAudioFinished.RemoveAll(this);
		ActiveFlashback = nullptr;
	}

	// 회수음이 끝났으니 보관해둔 조각의 스토리 컷 시작 신호
	if (PendingStoryDefinition) {
		UE_LOG(LogTemp, Warning, TEXT("[진단] 브로드캐스트, Story=%s"),
			PendingStoryDefinition->Story ? TEXT("있음") : TEXT("NULL"));
		UMemoryFragmentDefinition* Def = PendingStoryDefinition;
		PendingStoryDefinition = nullptr;
		OnFlashbackFinished.Broadcast(Def);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("[진단] PendingStoryDefinition NULL"));
	}
}
