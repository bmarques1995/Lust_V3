# Incorrect Vulkan Attribution

Since the commit `0e0a4614c0c45888ec8a235a70b99cc67036d025` until the commit `131ae25571ed78055a6c3d562826c82c1c82b6cd`, there is a compilation error, originated from the file `VKContext.cpp` line `approx. 817` inside `Lust::VKContext::CreateFramebuffers`, the snippet `framebufferInfo.m_Layers = 1;` must be replaced with `framebufferInfo.layers = 1;`, this error only was perceived at the point of the first commit of this file, and was originated from a wrong renaming by visual studio.

# Incorrect order of Source Config

Since the commit `73345ffa2818a1e00367f8505443d8a7446efab2` until the commit `131ae25571ed78055a6c3d562826c82c1c82b6cd`, there is a cmake error, where the imgui files are downloaded after the file regex, causing the ignoring at the first read of the file,

To fix this, move to, `Lust/CMakeLists.txt` cut the snippet:

```cmake

set(IMGUI_COMMIT_HASH "11b3a7c8ca23201294464c7f368614a9106af2a1")
set(IMGUI_SOURCE_URL "https://raw.githubusercontent.com/ocornut/imgui")

set(IMGUI_PUBLIC_FILES "imgui.h"
					   "imconfig.h"
					   "imgui.cpp"
					   "imstb_rectpack.h"
					   "imstb_textedit.h"
					   "imstb_truetype.h"
					   "imgui_demo.cpp"
					   "imgui_draw.cpp"
					   "imgui_widgets.cpp"
					   "imgui_tables.cpp" 
					   "imgui_internal.h")

set(IMGUI_BACKEND_FILES "backends/imgui_impl_sdl3.cpp"
						"backends/imgui_impl_sdl3.h"
						"backends/imgui_impl_vulkan.cpp"
						"backends/imgui_impl_vulkan.h")

if(WIN32)
	set(IMGUI_BACKEND_FILES ${IMGUI_BACKEND_FILES}
							"backends/imgui_impl_dx12.cpp"
							"backends/imgui_impl_dx12.h")
endif()

download_sources_from_git_repo(COMMIT_VALUE ${IMGUI_COMMIT_HASH} SOURCE_BASE_URL ${IMGUI_SOURCE_URL} SOURCE_INPUTS ${IMGUI_PUBLIC_FILES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui/public/imgui)
download_sources_from_git_repo(COMMIT_VALUE ${IMGUI_COMMIT_HASH} SOURCE_BASE_URL ${IMGUI_SOURCE_URL} SOURCE_INPUTS ${IMGUI_BACKEND_FILES} SOURCE_BASE_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/third_party/imgui)

```

and put it before:

```cmake

file(GLOB_RECURSE LUST_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.hpp")
file(GLOB_RECURSE LUST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")

```

# Wrong Time of Shader Compilation

Since the commit `076c6fdc0968df98b6e2474574eaed17ae0d634b` until the commit `131ae25571ed78055a6c3d562826c82c1c82b6cd`, there is a runtime error, caused by trying to start the Renderer2D without the compiled shaders, to fix this, go to `Application.cpp` at `Lust::Application::Application`, go to the following snippet:

```cpp

try
{
    m_SPVCompiler.reset(new SPVCompiler("_main", "_6_8", "1.3"));
    m_CSOCompiler.reset(new CSOCompiler("_main", "_6_8"));
    m_SPVCompiler->PushShaderPath("./assets/shaders/HelloTriangle.hlsl", PipelineType::Graphics);
    m_CSOCompiler->PushShaderPath("./assets/shaders/HelloTriangle.hlsl", PipelineType::Graphics);
    m_SPVCompiler->PushShaderPath("./assets/shaders/FlatColor.hlsl", PipelineType::Graphics);
    m_CSOCompiler->PushShaderPath("./assets/shaders/FlatColor.hlsl", PipelineType::Graphics);
    m_SPVCompiler->PushShaderPath("./assets/shaders/Renderer2D.hlsl", PipelineType::Graphics);
    m_CSOCompiler->PushShaderPath("./assets/shaders/Renderer2D.hlsl", PipelineType::Graphics);
    m_SPVCompiler->CompilePackedShader();
    m_CSOCompiler->CompilePackedShader();
}
catch (CompilerException e)
{
    Console::CoreError("{}", e.what());
}

```

between `m_Window->ResetTitle(buffer.str());` and `m_Instrumentator.reset(GPUInstrumentator::Instantiate(m_Context.get()));` and put it between `m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));` and `TextureLibrary::InitCopyPipeline(m_Context.get());`. 

- These steps should fix the previous broken versions.