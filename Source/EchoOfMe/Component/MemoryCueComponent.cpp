// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/MemoryCueComponent.h"

#include "Item/MemoryFragmentActor.h"
#include "Data/MemoryFragmentDefinition.h"


void UMemoryCueComponent::ResolveCueData()
{
	CueType = EListeningCue::Memory;

	if (const AMemoryFragmentActor* Frag = Cast<AMemoryFragmentActor>(GetOwner()))
	{
		if (const UMemoryFragmentDefinition* Def = Frag->GetMemoryDefinition())
		{
			AmbientSound   = Def->CueAmbientSound;
			ListeningSound = Def->CueListeningSound;
			if (Def->CueAttenuation) CueAttenuation = Def->CueAttenuation;
		}
	}
}
