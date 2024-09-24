// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class hl2da : ModuleRules
{
    private string BinariesPath = "../../Binaries/hl2da/";

    public hl2da(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);

        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, BinariesPath, "Microsoft.MixedReality.EyeTracking.dll"));
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, BinariesPath, "hl2da.dll"));
        RuntimeDependencies.Add(Path.Combine(ModuleDirectory, BinariesPath, "hl2da_itc.dll"));
    }
}
