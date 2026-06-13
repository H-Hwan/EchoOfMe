// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Component/ListeningComponent.h"

#include "CueComponentBase.generated.h"


class UAudioComponent;
class USoundBase;
class USoundAttenuation;
class APawn;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECHOOFME_API UCueComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCueComponentBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// ListeningComponent에 보고할 큐 종류
	UPROPERTY(EditAnywhere, Category = "Cue")
	EListeningCue CueType = EListeningCue::Memory;

	// 듣기 안 해도 들리는 상시음
	UPROPERTY(EditAnywhere, Category = "Cue|Sound")
	TObjectPtr<USoundBase> AmbientSound;

	// 듣기 시 크로스페이드되어 들리는 음
	UPROPERTY(EditAnywhere, Category = "Cue|Sound")
	TObjectPtr<USoundBase> ListeningSound;

	// 비우면 사운드 에셋 자체 감쇠 사용
	UPROPERTY(EditAnywhere, Category = "Cue|Sound")
	TObjectPtr<USoundAttenuation> CueAttenuation;

	// 이 거리 안 + 듣기 중일 때 듣기음 활성 (<=0 이면 거리 무시)
	UPROPERTY(EditAnywhere, Category = "Cue")
	float CueRange = 600.f;

	UPROPERTY(EditAnywhere, Category = "Cue")
	float CrossfadeTime = 0.3f;

	// 서브클래스가 사운드/타입 확정하는 훅 (BeginPlay 초반 호출)
	virtual void ResolveCueData() {}

private:
	UPROPERTY() TObjectPtr<UAudioComponent> AmbientAudio;
	UPROPERTY() TObjectPtr<UAudioComponent> ListeningAudio;
	UPROPERTY() TObjectPtr<UListeningComponent> ListeningComp;
	UPROPERTY() TObjectPtr<APawn> CachedPlayer;

	bool bRevealed = false;

	UAudioComponent* CreateCueAudio(USoundBase* Sound, bool bShouldAutoActivate);
	bool ComputeShouldReveal() const;
	void SetRevealed(bool bNewRevealed);
};
