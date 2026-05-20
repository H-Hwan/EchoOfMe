using UnrealBuildTool;
using System.Collections.Generic;

public class EchoOfMeEditorTarget : TargetRules
{
    public EchoOfMeEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;

        ExtraModuleNames.Add("EchoOfMe");
    }
}