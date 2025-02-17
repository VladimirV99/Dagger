using Sharpmake;
using System.IO;

[Generate]
public class MainProject : Project
{
    private string m_RootDirectory;
    private const string mc_ProjectName = "dagger";

    public MainProject()
    {
        // Eval paths
        m_RootDirectory = Path.Combine(this.SharpmakeCsPath, @"..\..");

        Name = mc_ProjectName;
        SourceRootPath = Path.Combine(m_RootDirectory, @"source", mc_ProjectName);

        SourceFilesExtensions.Add(".shader");
        SourceFilesExtensions.Add(".glsl");
        AdditionalSourceRootPaths.Add(Path.Combine(m_RootDirectory, @"data"));

        AddTargets(
            new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release
            ),
            new Target(
                Platform.win64,
                DevEnv.vs2022,
                Optimization.Debug | Optimization.Release
            )
        );
    }

    [Configure]
    public void ConfigureAll(Project.Configuration config, Target target)
    {
        config.ProjectFileName = @"[project.Name].[target.DevEnv]";
        config.ProjectPath = Path.Combine(m_RootDirectory, @"projects", mc_ProjectName);

        config.VcxprojUserFile = new Configuration.VcxprojUserFileSettings();
        config.VcxprojUserFile.LocalDebuggerWorkingDirectory = Path.Combine(m_RootDirectory, @"data");

        // Additional includes
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"source", mc_ProjectName));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"spdlog", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"simpleini"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"stb", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"entt", @"single_include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"fmt", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"glfw3", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"json", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"sparse-map", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"glad", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"imgui", @"include"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"imgui", @"include", @"imgui"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"imgui", @"include", @"imgui", @"backends"));
        config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"glm", @"include"));
		config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"soloud", @"include"));
		config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"soloud", @"src", @"audiosource", @"wav"));
		config.IncludePaths.Add(Path.Combine(m_RootDirectory, @"libs", @"soloud", @"src", @"backend", @"miniaudio"));

        // Add libs
        config.LibraryPaths.Add(Path.Combine(m_RootDirectory, @"libs", "glfw3-lib"));
        config.LibraryFiles.Add("glfw3.lib");
        config.LibraryFiles.Add("glu32.lib");
        config.LibraryFiles.Add("opengl32.lib");

        // For external CPP files
        string external = Path.Combine(m_RootDirectory, @"source", mc_ProjectName, @"external");
        if (!Directory.Exists(external))
        {
            Directory.CreateDirectory(external);
        }

        // IMGUI
        string imguiRoot = Path.Combine(m_RootDirectory, @"libs", @"imgui", @"include", @"imgui");
        string imguiDest = Path.Combine(external, @"imgui");

        if (!Directory.Exists(imguiDest))
        {
            Directory.CreateDirectory(imguiDest);
        }

        File.Copy(Path.Combine(imguiRoot, @"imgui.cpp"), Path.Combine(imguiDest, @"imgui.cpp"), true);
        File.Copy(Path.Combine(imguiRoot, @"imgui_widgets.cpp"), Path.Combine(imguiDest, @"imgui_widgets.cpp"), true);
        File.Copy(Path.Combine(imguiRoot, @"imgui_tables.cpp"), Path.Combine(imguiDest, @"imgui_tables.cpp"), true);
        File.Copy(Path.Combine(imguiRoot, @"imgui_draw.cpp"), Path.Combine(imguiDest, @"imgui_draw.cpp"), true);
        File.Copy(Path.Combine(imguiRoot, @"backends", @"imgui_impl_glfw.cpp"), Path.Combine(imguiDest, @"imgui_impl_glfw.cpp"), true);
        File.Copy(Path.Combine(imguiRoot, @"backends", @"imgui_impl_opengl3.cpp"), Path.Combine(imguiDest, @"imgui_impl_opengl3.cpp"), true);

        // GLAD
        string gladRoot = Path.Combine(m_RootDirectory, @"libs", @"glad", @"include", @"glad");
        string gladDest = Path.Combine(external, @"glad");

        if (!Directory.Exists(gladDest))
        {
            Directory.CreateDirectory(gladDest);
        }

        File.Copy(Path.Combine(gladRoot, @"glad.c"), Path.Combine(gladDest, @"glad.c"), true);

        // SOLOUD
        string soloudRoot = Path.Combine(m_RootDirectory, @"libs", @"soloud", @"src");
        string soloudDest = Path.Combine(external, @"soloud");

        if (!Directory.Exists(soloudDest))
        {
            Directory.CreateDirectory(soloudDest);
        }

        File.Copy(Path.Combine(soloudRoot, @"audiosource", @"wav", @"dr_impl.cpp"), Path.Combine(soloudDest, @"dr_impl.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"audiosource", @"wav", @"soloud_wav.cpp"), Path.Combine(soloudDest, @"soloud_wav.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"audiosource", @"wav", @"soloud_wavstream.cpp"), Path.Combine(soloudDest, @"soloud_wavstream.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"audiosource", @"wav", @"stb_vorbis.c"), Path.Combine(soloudDest, @"stb_vorbis.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"backend", @"miniaudio", @"soloud_miniaudio.cpp"), Path.Combine(soloudDest, @"soloud_miniaudio.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_audiosource.cpp"), Path.Combine(soloudDest, @"soloud_audiosource.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_bus.cpp"), Path.Combine(soloudDest, @"soloud_bus.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_3d.cpp"), Path.Combine(soloudDest, @"soloud_core_3d.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_basicops.cpp"), Path.Combine(soloudDest, @"soloud_core_basicops.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_faderops.cpp"), Path.Combine(soloudDest, @"soloud_core_faderops.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_filterops.cpp"), Path.Combine(soloudDest, @"soloud_core_filterops.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_getters.cpp"), Path.Combine(soloudDest, @"soloud_core_getters.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_setters.cpp"), Path.Combine(soloudDest, @"soloud_core_setters.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_voicegroup.cpp"), Path.Combine(soloudDest, @"soloud_core_voicegroup.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_core_voiceops.cpp"), Path.Combine(soloudDest, @"soloud_core_voiceops.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_fader.cpp"), Path.Combine(soloudDest, @"soloud_fader.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_fft_lut.cpp"), Path.Combine(soloudDest, @"soloud_fft_lut.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_fft.cpp"), Path.Combine(soloudDest, @"soloud_fft.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_file.cpp"), Path.Combine(soloudDest, @"soloud_file.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_filter.cpp"), Path.Combine(soloudDest, @"soloud_filter.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_misc.cpp"), Path.Combine(soloudDest, @"soloud_misc.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_queue.cpp"), Path.Combine(soloudDest, @"soloud_queue.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud_thread.cpp"), Path.Combine(soloudDest, @"soloud_thread.cpp"), true);
        File.Copy(Path.Combine(soloudRoot, @"core", @"soloud.cpp"), Path.Combine(soloudDest, @"soloud.cpp"), true);

        // Setup additional compiler options
        config.TargetPath = Path.Combine(m_RootDirectory, @"bin", config.Platform.ToString());

        config.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);
        config.Options.Add(Options.Vc.Compiler.CppLanguageStandard.CPP17);
        config.Options.Add(Options.Vc.Compiler.RuntimeLibrary.MultiThreadedDebugDLL);

        // Define math constants
        config.Defines.Add("_USE_MATH_DEFINES");

        // Define soloud backend
        config.Defines.Add("WITH_MINIAUDIO");

        // Define flags
        if (target.Optimization == Optimization.Debug)
        {
            config.Defines.Add("DAGGER_DEBUG");
        }

        if (target.Optimization == Optimization.Release)
        {
            config.Defines.Add("DAGGER_RELEASE");
        }
    }

    public override void PostResolve()
    {
        base.PostResolve();
    }
}

[Generate]
public class MainSolution : Solution
{
    private const string mc_SolutionName = "Dagger";
    private string m_RootDirectory = Path.Combine(@"[solution.SharpmakeCsPath]", @"\..\..");

    public MainSolution()
    {
        Name = mc_SolutionName;

        AddTargets(
            new Target(
                Platform.win64,
                DevEnv.vs2019,
                Optimization.Debug | Optimization.Release
            ),
            new Target(
                Platform.win64,
                DevEnv.vs2022,
                Optimization.Debug | Optimization.Release
            )
        );
    }

    [Configure]
    public void ConfigureAll(Solution.Configuration config, Target target)
    {
        config.SolutionPath = Path.Combine(m_RootDirectory, "projects");
        config.SolutionFileName = @"[solution.Name].[target.DevEnv]";
        config.Options.Add(Options.Vc.General.WindowsTargetPlatformVersion.Latest);

        config.AddProject<MainProject>(target);
    }
}

public static class Main
{
    [Sharpmake.Main]
    public static void SharpmakeMain(Arguments sharpmakeArgs)
    {
        sharpmakeArgs.Generate<MainSolution>();
    }
}
