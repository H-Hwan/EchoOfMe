// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/EquipmentComponent.h"

#include "Component/FlashlightComponent.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "Engine/World.h"


UEquipmentComponent::UEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}


void UEquipmentComponent::SetEquipmentMeshes(USceneComponent* InFlashlightMesh, USceneComponent* InRecorderMesh, UFlashlightComponent* InFlashlight)
{
    FlashlightMesh = InFlashlightMesh;
    RecorderMesh = InRecorderMesh;
    Flashlight = InFlashlight;
}

void UEquipmentComponent::RequestFlashlight()
{
    if (bIsSwitching) return;

    const EEquipmentSlot Target = (CurrentEquipment == EEquipmentSlot::Flashlight)
        ? EEquipmentSlot::None : EEquipmentSlot::Flashlight;

    StartSwitch(Target);
}


void UEquipmentComponent::RequestRecorder()
{
    if (bIsSwitching) return;

    const EEquipmentSlot Target = (CurrentEquipment == EEquipmentSlot::Recorder)
        ? EEquipmentSlot::None : EEquipmentSlot::Recorder;

    StartSwitch(Target);
}


void UEquipmentComponent::StartSwitch(EEquipmentSlot NewTarget)
{
    if (CurrentEquipment == NewTarget) return;

    bIsSwitching = true;
    TargetEquipment = NewTarget;

    // 현재 들고 있는 게 있으면 집어넣기부터 처리
    if (CurrentEquipment != EEquipmentSlot::None)
    {
        HolsteringEquipment = CurrentEquipment;
        CurrentEquipment = EEquipmentSlot::None;

        // 손전등을 집어넣을 때 자동 OFF
        if (HolsteringEquipment == EEquipmentSlot::Flashlight && Flashlight)
        {
            Flashlight->SetFlashLightOn(false);
        }

        UAnimMontage* HolsterMontage = (HolsteringEquipment == EEquipmentSlot::Flashlight)
            ? FlashlightHolsterMontage
            : RecorderHolsterMontage;

        PlayMontage(HolsterMontage, &UEquipmentComponent::HandleHolsterFinished);
    }
    else
    {
        // 빈손이었으면 바로 꺼내기
        HandleHolsterFinished();
    }
}


void UEquipmentComponent::HandleHolsterFinished()
{
    // 집어넣기가 끝나면 메시를 숨기고 빈손 상태로 전환
    if (HolsteringEquipment != EEquipmentSlot::None)
    {
        ShowMesh(HolsteringEquipment, false);
        HolsteringEquipment = EEquipmentSlot::None;
    }

    // 빈손이 최종 목표면 여기서 끝
    if (TargetEquipment == EEquipmentSlot::None)
    {
        bIsSwitching = false;
        OnEquipmentChanged.Broadcast(EEquipmentSlot::None);
        return;
    }

    // 새 장비 꺼내기
    UAnimMontage* DrawMontage = (TargetEquipment == EEquipmentSlot::Flashlight)
        ? FlashlightDrawMontage
        : RecorderDrawMontage;

    ShowMesh(TargetEquipment, true);
    CurrentEquipment = TargetEquipment;

    PlayMontage(DrawMontage, &UEquipmentComponent::HandleDrawFinished);
}


void UEquipmentComponent::HandleDrawFinished()
{
    bIsSwitching = false;

    OnEquipmentChanged.Broadcast(CurrentEquipment);
}


void UEquipmentComponent::ShowMesh(EEquipmentSlot Slot, bool bVisible)
{
    USceneComponent* Target = nullptr;

    switch (Slot)
    {
        case EEquipmentSlot::Flashlight: Target = FlashlightMesh; break;
        case EEquipmentSlot::Recorder:   Target = RecorderMesh; break;
        default: return;
    }

    if (Target)
    {
        Target->SetVisibility(bVisible, false);
    }
}


void UEquipmentComponent::PlayMontage(UAnimMontage* Montage, void (UEquipmentComponent::*Callback)())
{
    // 몽타주가 없으면 즉시 콜백해서 미할당 상태에서도 흐름은 유지한다.
    if (!Montage)
    {
        (this->*Callback)();
        return;
    }

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (!OwnerChar)
    {
        (this->*Callback)();
        return;
    }

    USkeletalMeshComponent* SkelMesh = OwnerChar->GetMesh();
    if (!SkelMesh)
    {
        (this->*Callback)();
        return;
    }

    UAnimInstance* AnimInst = SkelMesh->GetAnimInstance();
    if (!AnimInst)
    {
        (this->*Callback)();
        return;
    }

    const float Duration = AnimInst->Montage_Play(Montage, 1.f);

    if (Duration <= 0.f)
    {
        // 재생 실패 시 즉시 콜백
        (this->*Callback)();
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        (this->*Callback)();
        return;
    }

    World->GetTimerManager().SetTimer(
        MontageTimerHandle,
        FTimerDelegate::CreateUObject(this, Callback),
        Duration,
        false
    );
}
