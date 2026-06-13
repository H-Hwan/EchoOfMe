// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/CueComponentBase.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"


// Sets default values for this component's properties
UCueComponentBase::UCueComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCueComponentBase::BeginPlay()
{
	Super::BeginPlay();

	ResolveCueData();

	CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	if (CachedPlayer)
	{
		ListeningComp = CachedPlayer->FindComponentByClass<UListeningComponent>();
	}

	// 상시 재생
	if (AmbientSound) AmbientAudio = CreateCueAudio(AmbientSound, true);
	// 듣기 시 페이드 인
	if (ListeningSound) ListeningAudio = CreateCueAudio(ListeningSound, false);
}


void UCueComponentBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AmbientAudio) AmbientAudio->Stop();
	if (ListeningAudio) ListeningAudio->Stop();

	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UCueComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ListeningComp)
	{
		if (!CachedPlayer) CachedPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
		if (CachedPlayer) ListeningComp = CachedPlayer->FindComponentByClass<UListeningComponent>();
	}

	SetRevealed(ComputeShouldReveal());
}


UAudioComponent* UCueComponentBase::CreateCueAudio(USoundBase* Sound, bool bShouldAutoActivate)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Sound) return nullptr;

	UAudioComponent* AC = NewObject<UAudioComponent>(Owner);
	if (!AC) return nullptr;

	AC->bAutoActivate = bShouldAutoActivate;
	AC->SetSound(Sound);
	if (CueAttenuation) AC->AttenuationSettings = CueAttenuation;

	AC->SetupAttachment(Owner->GetRootComponent());
	AC->RegisterComponent();
	return AC;
}


bool UCueComponentBase::ComputeShouldReveal() const
{
	if (!ListeningComp || !CachedPlayer)	return false;
	if (!ListeningComp->IsListening())		return false;
	// 거리 무시
	if (CueRange <= 0.f)					return true;

	const AActor* Owner = GetOwner();
	if (!Owner) return false;

	const float DistSq = FVector::DistSquared(Owner->GetActorLocation(), CachedPlayer->GetActorLocation());
	return DistSq <= FMath::Square(CueRange);
}


void UCueComponentBase::SetRevealed(bool bNewRevealed)
{
	if (bRevealed == bNewRevealed) return;
	bRevealed = bNewRevealed;

	if (bNewRevealed)
	{
		if (ListeningAudio) ListeningAudio->FadeIn(CrossfadeTime, 1.f);
		if (AmbientAudio) AmbientAudio->FadeOut(CrossfadeTime, 0.f);
		if (ListeningComp) ListeningComp->SetCurrentCue(CueType);
	}
	else
	{
		if (ListeningAudio) ListeningAudio->FadeOut(CrossfadeTime, 0.f);
		if (AmbientAudio) AmbientAudio->FadeIn(CrossfadeTime, 1.f);
		if (ListeningComp && ListeningComp->GetCurrentCue() == CueType)
		{
			ListeningComp->SetCurrentCue(EListeningCue::None);
		}
	}
}
