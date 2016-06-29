

namespace UnrealBuildTool.Rules
{
	public class TreeArchitectEditor : ModuleRules
	{
		public TreeArchitectEditor(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
                new string[] {
					"TreeArchitectEditor/Private",
				    "TreeArchitectRuntime/Private",
					// ... add other private include paths required here ...
				}
				);

            PrivateIncludePathModuleNames.AddRange(
                new string[] {
				"Settings",
				"IntroTutorials",
                "AssetTools"
			}
            );

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
                    "Slate",
                    "EditorStyle",
                    "UnrealEd",
				    "KismetWidgets",
                    "GraphEditor",
                    "RHI",
                    "RenderCore",
                    "ShaderCore",
                    "Landscape",
				    "Kismet",  // for FWorkflowCentricApplication
                    "Projects",
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
                    "InputCore",
                    "SlateCore",
				    "RenderCore",
                    "PropertyEditor",
                    "WorkspaceMenuStructure",
                    "LevelEditor",
				    "EditorStyle",
				    "ContentBrowser",
                    "TreeArchitectRuntime",
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}