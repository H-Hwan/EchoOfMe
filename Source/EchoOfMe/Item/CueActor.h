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
		ACueActor();

protected:
	// 게임 시작 또는 스폰 시 호출
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

