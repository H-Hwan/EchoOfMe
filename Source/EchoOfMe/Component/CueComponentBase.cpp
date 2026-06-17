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

	RefreshPlayerCache();

	// 상시 재생
	if (AmbientSound) AmbientAudio = CreateCueAudio(AmbientSound, false);
	// 듣기 시 페이드 인
	if (ListeningSound) ListeningAudio = CreateCueAudio(ListeningSound, false);

	const bool bShouldReveal = ComputeShouldReveal();
	SetRevealed(bShouldReveal);
	SetAmbientAudible(ComputeIsInRange() && !bShouldReveal);
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

	RefreshPlayerCache();

	const bool bShouldReveal = ComputeShouldReveal();
	SetRevealed(bShouldReveal);
	SetAmbientAudible(ComputeIsInRange() && !bShouldReveal);
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


void UCueComponentBase::RefreshPlayerCache()
{
	APawn* CurrentPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	if (CurrentPlayer == CachedPlayer && IsValid(CachedPlayer) && IsValid(ListeningComp)) return;

	CachedPlayer = CurrentPlayer;
	ListeningComp = IsValid(CachedPlayer) ? CachedPlayer->FindComponentByClass<UListeningComponent>() : nullptr;
}


bool UCueComponentBase::ComputeIsInRange() const
{
	if (!IsValid(CachedPlayer))			return false;

	// 거리 무시
	if (CueRange <= 0.f)					return true;

	const AActor* Owner = GetOwner();
	if (!Owner) return false;

	const float DistSq = FVector::DistSquared(Owner->GetActorLocation(), CachedPlayer->GetActorLocation());
	return DistSq <= FMath::Square(CueRange);
}


bool UCueComponentBase::ComputeShouldReveal() const
{
	if (!IsValid(ListeningComp))			return false;
	if (!ListeningComp->IsListening())	return false;

	return ComputeIsInRange();
}


void UCueComponentBase::SetRevealed(bool bNewRevealed)
{
	if (bRevealed == bNewRevealed) return;
	bRevealed = bNewRevealed;

	if (bNewRevealed)
	{
		if (ListeningAudio) ListeningAudio->FadeIn(CrossfadeTime, 1.f);
		if (IsValid(ListeningComp)) ListeningComp->SetCurrentCue(CueType);
	}
	else
	{
		if (ListeningAudio) ListeningAudio->FadeOut(CrossfadeTime, 0.f);
		if (IsValid(ListeningComp) && ListeningComp->GetCurrentCue() == CueType)
		{
			ListeningComp->SetCurrentCue(EListeningCue::None);
		}
	}
}


void UCueComponentBase::SetAmbientAudible(bool bNewAmbientAudible)
{
	if (bAmbientAudible == bNewAmbientAudible) return;
	bAmbientAudible = bNewAmbientAudible;

	if (!AmbientAudio) return;

	if (bNewAmbientAudible)
	{
		AmbientAudio->FadeIn(CrossfadeTime, 1.f);
	}
	else
	{
		AmbientAudio->FadeOut(CrossfadeTime, 0.f);
	}
}
