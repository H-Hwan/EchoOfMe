#include "Item/CueActor.h"

#include "Component/ListeningComponent.h"

#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Pawn.h"


ACueActor::ACueActor() {
	PrimaryActorTick.bCanEverTick = false;

	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	SetRootComponent(TriggerSphere);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetSphereRadius(600.f);   // 큐가 인지되는 최대 거리

	CueAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("CueAudio"));
	CueAudio->SetupAttachment(RootComponent);
	CueAudio->bAutoActivate = false;   // 듣기 켜야 시작
}

// 게임 시작 또는 스폰 시 호출
void ACueActor::BeginPlay() {
	Super::BeginPlay();

	if (CueSound) CueAudio->SetSound(CueSound);

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ACueActor::OnTriggerBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ACueActor::OnTriggerEnd);
}

void ACueActor::OnTriggerBegin(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*,
							   int32, bool, const FHitResult&)
{
	APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn) return;

	UListeningComponent* Listening = Pawn->FindComponentByClass<UListeningComponent>();
	if (!Listening) return;

	bPlayerInRange = true;
	ActiveListening = Listening;
	Listening->OnListeningChanged.AddDynamic(this, &ACueActor::HandleListeningChanged);

	// 이미 듣고 있는 상태에서 영역 진입한 경우 즉시 큐 시작
	if (Listening->IsListening())
	{
		HandleListeningChanged(true);
	}
}

void ACueActor::OnTriggerEnd(UPrimitiveComponent*, AActor* Other, UPrimitiveComponent*, int32) {
	APawn* Pawn = Cast<APawn>(Other);
	if (!Pawn) return;

	bPlayerInRange = false;

	if (ActiveListening) {
		// 영역 나가면 큐 중단, 현재 큐 비움
		if (CueAudio && CueAudio->IsPlaying()) {
			CueAudio->FadeOut(0.3f, 0.f);
		}
		if (ActiveListening->GetCurrentCue() == CueType) {
			ActiveListening->SetCurrentCue(EListeningCue::None);
		}

		ActiveListening->OnListeningChanged.RemoveDynamic(this, &ACueActor::HandleListeningChanged);
		ActiveListening = nullptr;
	}
}

void ACueActor::HandleListeningChanged(bool bIsListening) {
	if (!bPlayerInRange) return;
	if (!CueAudio || !CueSound) return;

	if (bIsListening) {
		CueAudio->FadeIn(0.3f, 1.f);
		if (ActiveListening) {
			ActiveListening->SetCurrentCue(CueType);
		}
	}
	else {
		CueAudio->FadeOut(0.3f, 0.f);
		if (ActiveListening && ActiveListening->GetCurrentCue() == CueType) {
			ActiveListening->SetCurrentCue(EListeningCue::None);
		}
	}
}
