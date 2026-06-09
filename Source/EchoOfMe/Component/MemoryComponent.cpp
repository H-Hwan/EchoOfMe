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
	}

	/*	[ToDo] 자막 표시
		Definition->FlashbackText
		>> 사운드가 있으면 사운드 길이 따라가도록	*/
	UE_LOG(LogTemp, Log, TEXT("[Memory] 자막: %s"), *Definition->FlashbackText.ToString());

	OnMemoryCollected.Broadcast(Definition);
}


void UMemoryComponent::HandleFlashbackFinished() {
	if (ActiveFlashback) {
		ActiveFlashback->OnAudioFinished.RemoveAll(this);
		ActiveFlashback = nullptr;
	}
}
