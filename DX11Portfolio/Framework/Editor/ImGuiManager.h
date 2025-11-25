#pragma once

struct ImGUIDesc
{
    HWND Hwnd;
    ComPtr<ID3D11Device> Device;
    ComPtr<ID3D11DeviceContext> DeviceContext;

    float Width;
    float Height;
};

class ImGuiManager
{
public:
    static ImGuiManager* Get();

public:
    static void SetDesc(const ImGUIDesc& InDesc);
    static void Create();
    static void Shutdown();
    static void Destroy();

public:
    void BeginFrame();
    void EndFrame();
    void RenderDrawData(ID3D11DeviceContext* context);

    void ShowSimpleDockUI();
    void GizmoTest();

public:
    void RenderText(float x, float y, float r, float g, float b, string content);
    void RenderText(float x, float y, float r, float g, float b, float a, string content);

private:
    static bool initialized;
    static ImGUIDesc ImGuiDesc;
    static ImGuiManager* Instance;

private:    
    void Initialize();

private:
    ImGuiManager();
    ~ImGuiManager();

private:
    struct FGuiText
    {
        Vector2 Position;
        FColor Color;
        string Content;
    };

private:
    vector<FGuiText> Contents;

private:
    std::mutex drawDataMutex;
    ImDrawData* drawDataBuffers[3] = { nullptr, nullptr, nullptr };
    int writeIndex = 1;     // 게임 쓰레드가 쓸 인덱스
    int renderedIndex = 0;  // 렌더 쓰레드가 그릴 인덱스

private:
    ImDrawData* CloneDrawData(ImDrawData* src);
    void FreeDrawData(ImDrawData* drawData);
};
