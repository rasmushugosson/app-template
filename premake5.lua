local function is_linux()
	return os.target() == "linux"
end

local function is_windows()
	return os.target() == "windows"
end

local function is_macos()
	return os.target() == "macosx"
end

local function is_clang()
	return _OPTIONS["cc"] == "clang" or _OPTIONS["cc"] == "clangcl"
end

local function workspace_location()
	if is_windows() then
		return "build/windows"
	elseif is_linux() then
		if is_clang() then
			return "build/linux-clang"
		else
			return "build/linux-gcc"
		end
	elseif is_macos() then
		if is_clang() then
			return "build/macos-clang"
		else
			return "build/macos"
		end
	end
	return "build"
end

local vulkanSDK = os.getenv("VULKAN_SDK") or os.getenv("VK_SDK_PATH")

local function detect_vulkan()
	if vulkanSDK then
		return true, vulkanSDK
	end
	if os.target() == "linux" or os.target() == "macosx" then
		if os.isfile("/usr/include/vulkan/vulkan.h") then
			return true, nil
		end
	end
	return false, nil
end

local vulkanAvailable, vulkanSDKPath = detect_vulkan()

if vulkanAvailable then
	if vulkanSDKPath then
		print(">> Vulkan SDK detected: " .. vulkanSDKPath)
	else
		print(">> System Vulkan detected")
	end
end

-- Directory paths
local dep_dir = "dep"
local log_lib_dir = dep_dir .. "/log-lib"
local event_lib_dir = dep_dir .. "/event-lib"
local ecs_lib_dir = dep_dir .. "/ecs-lib"
local app_lib_dir = dep_dir .. "/app-lib"
local vendor_dir = app_lib_dir .. "/vendor"
local glad_dir = vendor_dir .. "/glad"
local glfw_dir = app_lib_dir .. "/dep/GLFW"
local openal_dir = app_lib_dir .. "/dep/OpenALSoft"

workspace("AppTemplate")
location(workspace_location())
architecture("x64")
configurations({ "Debug", "Release", "Dist" })
warnings("Extra")

filter("system:windows")
defines({ "AE_WINDOWS" })

filter("system:macosx")
defines({ "AE_MACOS" })

filter("system:linux")
defines({ "AE_LINUX" })

filter({})

filter("configurations:Debug")
defines({ "AE_DEBUG" })
symbols("On")

filter("configurations:Release")
defines({ "AE_RELEASE", "NDEBUG" })
optimize("Speed")

filter("configurations:Dist")
defines({ "AE_DIST", "NDEBUG" })
optimize("Speed")
symbols("Off")

filter({})

filter({ "action:vs*", "configurations:Debug" })
runtime("Debug")

filter({ "action:vs*", "configurations:Release or configurations:Dist" })
runtime("Release")

filter("action:vs*")
startproject("Template")
filter({})

filter({ "system:linux or system:macosx" })
if is_clang() then
	print(">> Using Clang toolchain")
	buildoptions({
		"-Wall",
		"-Wextra",
		"-Wpedantic",
		"-fcolor-diagnostics",
	})
else
	print(">> Using GCC toolchain")
	buildoptions({
		"-Wall",
		"-Wextra",
		"-Wpedantic",
		"-fdiagnostics-color=always",
	})
end
filter({})

-- Log library
project("Log")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	log_lib_dir .. "/log-lib/src/**.cpp",
	log_lib_dir .. "/log-lib/src/**.h",
	log_lib_dir .. "/log-lib/include/**.h",
})

includedirs({
	log_lib_dir .. "/log-lib/include",
	log_lib_dir .. "/log-lib/src",
})

pchheader(path.getabsolute(log_lib_dir .. "/log-lib/src/general/pch.h"))
pchsource(log_lib_dir .. "/log-lib/src/general/pch.cpp")

-- ECS library
project("ECS")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	ecs_lib_dir .. "/ecs-lib/src/**.cpp",
	ecs_lib_dir .. "/ecs-lib/src/**.h",
	ecs_lib_dir .. "/ecs-lib/include/**.h",
})

includedirs({
	log_lib_dir .. "/log-lib/include",
	ecs_lib_dir .. "/ecs-lib/include",
	ecs_lib_dir .. "/ecs-lib/src",
})

links({ "Log" })

pchheader(path.getabsolute(ecs_lib_dir .. "/ecs-lib/src/general/pch.h"))
pchsource(ecs_lib_dir .. "/ecs-lib/src/general/pch.cpp")

-- Event library
project("Event")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	event_lib_dir .. "/event-lib/src/**.cpp",
	event_lib_dir .. "/event-lib/src/**.h",
	event_lib_dir .. "/event-lib/include/**.h",
})

includedirs({
	log_lib_dir .. "/log-lib/include",
	event_lib_dir .. "/event-lib/include",
	event_lib_dir .. "/event-lib/src",
})

links({ "Log" })

pchheader(path.getabsolute(event_lib_dir .. "/event-lib/src/general/pch.h"))
pchsource(event_lib_dir .. "/event-lib/src/general/pch.cpp")

-- STB library (from app-lib vendor)
project("STB")
kind("StaticLib")
language("C")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	vendor_dir .. "/stb/**.c",
	vendor_dir .. "/stb/**.h",
})

includedirs({
	vendor_dir .. "/stb",
})

-- GLM (header-only utility project)
project("GLM")
kind("Utility")
language("C++")

files({
	vendor_dir .. "/glm/**.hpp",
	vendor_dir .. "/glm/**.h",
})

includedirs({
	vendor_dir .. "/glm",
})

-- Nlohmann (header-only utility project)
project("Nlohmann")
kind("Utility")
language("C++")

files({
	vendor_dir .. "/nlohmann/**.hpp",
})

includedirs({
	vendor_dir .. "/nlohmann",
})

-- GLAD library
project("GLAD")
kind("StaticLib")
language("C")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	glad_dir .. "/src/glad.c",
	glad_dir .. "/include/**.h",
})

includedirs({
	glad_dir .. "/include",
})

-- ImGui library
project("ImGui")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	vendor_dir .. "/imgui/*.cpp",
	vendor_dir .. "/imgui/*.h",
	vendor_dir .. "/imgui/backends/imgui_impl_glfw.cpp",
	vendor_dir .. "/imgui/backends/imgui_impl_glfw.h",
	vendor_dir .. "/imgui/backends/imgui_impl_opengl3.cpp",
	vendor_dir .. "/imgui/backends/imgui_impl_opengl3.h",
	vendor_dir .. "/imgui/backends/imgui_impl_opengl3_loader.h",
})

includedirs({
	vendor_dir,
	vendor_dir .. "/imgui",
})

filter("system:windows")
includedirs({ glfw_dir .. "/include" })
filter({})

if vulkanAvailable then
	files({
		vendor_dir .. "/imgui/backends/imgui_impl_vulkan.cpp",
		vendor_dir .. "/imgui/backends/imgui_impl_vulkan.h",
	})
	if vulkanSDKPath then
		includedirs({ vulkanSDKPath .. "/Include" })
	end
end

-- App library
project("App")
kind("StaticLib")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({
	app_lib_dir .. "/app-lib/src/**.cpp",
	app_lib_dir .. "/app-lib/src/**.h",
	app_lib_dir .. "/app-lib/include/**.h",
})

includedirs({
	glad_dir .. "/include",
	openal_dir .. "/include",
	log_lib_dir .. "/log-lib/include",
	event_lib_dir .. "/event-lib/include",
	app_lib_dir .. "/app-lib/include",
	app_lib_dir .. "/app-lib/src",
	vendor_dir .. "/glm",
	vendor_dir .. "/stb",
	vendor_dir .. "/imgui",
	vendor_dir .. "/nlohmann",
})

filter("system:windows")
includedirs({ glfw_dir .. "/include" })
filter({})

dependson({ "GLM", "Nlohmann" })

filter("system:windows")
links({
	glfw_dir .. "/lib/glfw3.lib",
	"opengl32.lib",
	"Log",
	"Event",
	"STB",
	"ImGui",
	"GLAD",
})

filter({ "system:windows", "configurations:Debug" })
links({ openal_dir .. "/lib/Debug/OpenAL32" })

filter({ "system:windows", "configurations:Release or configurations:Dist" })
links({ openal_dir .. "/lib/Release/OpenAL32" })

filter("system:linux")
links({
	"glfw",
	"GL",
	"openal",
	"Log",
	"Event",
	"STB",
	"ImGui",
	"GLAD",
})

filter("system:macosx")
links({
	"glfw",
	"OpenGL.framework",
	"openal",
	"Log",
	"Event",
	"STB",
	"ImGui",
	"GLAD",
})

filter({})

if vulkanAvailable then
	defines({ "AE_VULKAN" })
	if vulkanSDKPath then
		includedirs({ vulkanSDKPath .. "/Include" })
		filter("system:windows")
		links({ vulkanSDKPath .. "/Lib/vulkan-1.lib" })
		filter({})
	end

	filter("system:linux or system:macosx")
	links({ "vulkan" })

	filter({})
end

pchheader(path.getabsolute(app_lib_dir .. "/app-lib/src/general/pch.h"))
pchsource(app_lib_dir .. "/app-lib/src/general/pch.cpp")

-- Sandbox project (main application)
project("Sandbox")
kind("ConsoleApp")
language("C++")
cppdialect("C++23")
objdir("obj/%{prj.name}/%{cfg.buildcfg}")
targetdir("bin/%{prj.name}/%{cfg.buildcfg}")

files({ "sandbox/src/**.cpp", "sandbox/src/**.h" })

includedirs({
	glad_dir .. "/include",
	openal_dir .. "/include",
	log_lib_dir .. "/log-lib/include",
	event_lib_dir .. "/event-lib/include",
	ecs_lib_dir .. "/ecs-lib/include",
	app_lib_dir .. "/app-lib/include",
	"sandbox/src",
	vendor_dir .. "/glm",
	vendor_dir .. "/imgui",
	vendor_dir .. "/nlohmann",
})

filter("system:windows")
includedirs({ glfw_dir .. "/include" })
filter({})

dependson({ "GLM", "Nlohmann" })

filter("system:windows")
links({
	glfw_dir .. "/lib/glfw3.lib",
	"opengl32.lib",
	"Log",
	"Event",
	"ECS",
	"STB",
	"ImGui",
	"GLAD",
	"App",
})

filter({ "system:windows", "configurations:Debug" })
links({ openal_dir .. "/lib/Debug/OpenAL32" })
postbuildcommands({
	'{COPY} "%{wks.location}/../../' .. openal_dir .. '/lib/Debug/OpenAL32.dll" "%{cfg.targetdir}"',
})

filter({ "system:windows", "configurations:Release or configurations:Dist" })
links({ openal_dir .. "/lib/Release/OpenAL32" })
postbuildcommands({
	'{COPY} "%{wks.location}/../../' .. openal_dir .. '/lib/Release/OpenAL32.dll" "%{cfg.targetdir}"',
})

filter("system:linux")
links({
	"App",
	"ImGui",
	"STB",
	"GLAD",
	"Event",
	"ECS",
	"Log",
	"glfw",
	"GL",
	"openal",
})

filter("system:macosx")
links({
	"App",
	"ImGui",
	"STB",
	"GLAD",
	"Event",
	"ECS",
	"Log",
	"glfw",
	"OpenGL.framework",
	"openal",
})

filter({})

if vulkanAvailable then
	defines({ "AE_VULKAN" })
	if vulkanSDKPath then
		includedirs({ vulkanSDKPath .. "/Include" })
		filter("system:windows")
		links({ vulkanSDKPath .. "/Lib/vulkan-1.lib" })
		filter({})
	end

	filter("system:linux or system:macosx")
	links({ "vulkan" })

	filter({})
end

pchheader(path.getabsolute("sandbox/src/pch.h"))
pchsource("sandbox/src/pch.cpp")

-- Custom actions
local function own_source_files()
	local files = {}

	local function add(glob)
		for _, f in ipairs(os.matchfiles(glob)) do
			table.insert(files, f)
		end
	end

	add("sandbox/src/**.cpp")
	add("sandbox/src/**.c")
	add("sandbox/src/**.h")
	add("sandbox/src/**.hpp")

	return files
end

newaction({
	trigger = "format",
	description = "Run clang-format on all project source files",

	execute = function()
		local files = own_source_files()
		if #files == 0 then
			print("No source files found to format.")
			return
		end

		local cmd = "clang-format -i"

		for _, f in ipairs(files) do
			cmd = cmd .. " " .. f
		end

		print("Running:", cmd)
		os.execute(cmd)
	end,
})

local function run_clang_tidy(fix_mode)
	local all_files = own_source_files()
	local cpp_files = {}

	for _, f in ipairs(all_files) do
		local ext = path.getextension(f)
		if ext == ".cpp" or ext == ".cc" or ext == ".cxx" then
			table.insert(cpp_files, f)
		end
	end

	if #cpp_files == 0 then
		print("No C++ source files found to lint.")
		return
	end

	local build_dir = "."

	local mode_label = fix_mode and "lint + fix" or "lint"
	print(string.format("Running clang-tidy (%s) on %d files...", mode_label, #cpp_files))

	for _, f in ipairs(cpp_files) do
		local extra = fix_mode and "-fix -fix-errors" or ""
		local cmd = string.format('clang-tidy -quiet %s -p "%s" "%s"', extra, build_dir, f)

		local result = os.execute(cmd)

		if result ~= 0 then
			print("")
			if fix_mode then
				print("clang-tidy reported issues (and may have applied fixes) in " .. f .. ".")
			else
				print("Issues found in " .. f .. " (see diagnostics above).")
			end
			print("")
		end
	end

	if fix_mode then
		print("clang-tidy auto-fix pass completed. Review changes with git diff.")
	else
		print("clang-tidy lint completed.")
	end
end

newaction({
	trigger = "lint",
	description = "Run clang-tidy on all project C++ source files",
	execute = function()
		run_clang_tidy(false)
	end,
})

newaction({
	trigger = "lint-fix",
	description = "Run clang-tidy with automatic fixes on all project C++ source files",
	execute = function()
		print("NOTE: This will modify your source files in-place. Make sure your tree is clean (git status).")
		run_clang_tidy(true)
	end,
})

newaction({
	trigger = "gmake-gcc",
	description = "Generate GNU Makefiles (gmake) using GCC on *nix",
	execute = function()
		os.execute("premake5 gmake --cc=gcc")
	end,
})

newaction({
	trigger = "gmake-clang",
	description = "Generate GNU Makefiles (gmake) using Clang on *nix",
	execute = function()
		os.execute("premake5 gmake --cc=clang")
	end,
})
