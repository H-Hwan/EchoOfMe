// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EchoOfMe : ModuleRules
{
	public EchoOfMe(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTags", "UMG"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"Slate", "SlateCore"
		});

		// Slate UI 사용 시 활성화
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// 온라인 기능 사용 시 활성화
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// Steam 사용 시 uproject 플러그인 목록에서 OnlineSubsystemSteam 활성화 필요
	}
}
