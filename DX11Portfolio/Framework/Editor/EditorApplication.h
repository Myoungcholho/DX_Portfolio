#pragma once

#include <map>
#include <filesystem>

class EditorApplication
{
public:
	enum class eState
	{
		Disable,
		Active,
		Destroy,
	};

	template <typename T>
	T* GetWindow(const std::wstring& name)
	{
		auto iter = mEditorWindows.find(name);
		if (iter == mEditorWindows.end())
			return nullptr;

		return dynamic_cast<T*>(iter->second);
	}

	///<summary>
	///에디터를 초기화합니다.
	///</summary>
	static bool Initialize();

	/// <summary>
	/// 에디터 업데이트
	/// </summary>
	static void Run();

	/// <summary>
	/// 에디터를 로직을 업데이트합니다.
	/// </summary>
	static void Update();

	/// <summary>
	/// 에디터 렌더링을 업데이트합니다.
	/// </summary>
	static void OnGUI();

	/// <summary>
	/// 에디터를 종료합니다.
	/// </summary>
	static void Release();

	static void OpenProject();
	static void NewScene();
	static void SaveScene();
	static void SaveSceneAs();
	static void OpenScene(const std::filesystem::path& path);
	
public:
	static void SetWorld(UWorld* InWorld) { world = InWorld; }
	static void SetRenderer(URenderer* InRenderer) { renderer = InRenderer; }

private:
	static bool imGguiInitialize();
	static void imGuiRender();

	static ImGuiWindowFlags mFlag;
	static ImGuiDockNodeFlags mDockspaceFlags;
	static eState mState;
	static bool mFullScreen;
	static bool mPadding;
	static Vector2 mViewportBounds[2];
	static Vector2 mViewportSize;
	static bool mViewportFocused;
	static bool mViewportHovered;

	static std::map<std::wstring, EditorWindow*> mEditorWindows;

private:
	static UWorld* world;
	static URenderer* renderer;
};