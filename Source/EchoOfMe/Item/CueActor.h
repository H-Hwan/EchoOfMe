// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Component/ListeningComponent.h"

#include "CueActor.generated.h"


class USphereComponent;
class UAudioComponent;
class USoundBase;


UCLASS()
class ECHOOFME_API ACueActor : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACueActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
						int32 BodyIndex, bool bFromSweep, const FHitResult& Sweep);
	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* Comp, AActor* Other, UPrimitiveComponent* OtherComp,
					  int32 BodyIndex);

	UFUNCTION()
	void HandleListeningChanged(bool bIsListening);

	UPROPERTY(VisibleAnywhere, Category="Cue")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(VisibleAnywhere, Category="Cue")
	TObjectPtr<UAudioComponent> CueAudio;

	// 현재 큐의 종류 — 듣기 컴포넌트에 보고할 분류
	UPROPERTY(EditAnywhere, Category="Cue")
	EListeningCue CueType = EListeningCue::Memory;

	// 큐 사운드 — 루프 가능한 사운드
	UPROPERTY(EditAnywhere, Category="Cue")
	TObjectPtr<USoundBase> CueSound;

private:
	UPROPERTY()
	TObjectPtr<UListeningComponent> ActiveListening;

	bool bPlayerInRange = false;

};

