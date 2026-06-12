// Fill out your copyright notice in the Description page of Project Settings.


#include "StoryWidget.h"
#include "Data/StorySequence.h"


bool UStoryWidget::GetPage(int32 Index, FStoryPage& OutPage) const
{
	if (Pages.IsValidIndex(Index))
	{
		OutPage = Pages[Index];
		return true;
	}

	// 유효하지 않은 인덱스라면 False 반환
	return false;
}

//void UStoryWidget::ShowPage()
//{
//}
//
//void UStoryWidget::FinishSequence()
////{
////	if (IsValid(CurrentVoiceAudio))
////	{
////		CurrentVoiceAudio->Stop();
////	}
////
////}
//void UStoryWidget::PlaySequence()
//{
//}
