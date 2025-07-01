#pragma once

struct ImGUIDesc
{
    HWND Hwnd;
    ComPtr<ID3D11Device> Device;
    ComPtr<ID3D11DeviceContext> DeviceContext;

    /*ID3D11Device* Device;
    ID3D11DeviceContext* DeviceContext;*/

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

    // Á¾·á ½Ã
    static void Shutdown();
    static void Destroy();

public:
    static void BeginFrame();
    static void EndFrame();

public:
    void Render(); 

public:
    void RenderText(float x, float y, float r, float g, float b, string content);
    void RenderText(float x, float y, float r, float g, float b, float a, string content);

private:
    static bool initialized;
    static ImGUIDesc ImGuiDesc;

private:
    void Initialize();

private:
    ImGuiManager();
    ~ImGuiManager();

private:
    static ImGuiManager* Instance;

private:
    struct FGuiText
    {
        Vector2 Position;
        FColor Color;
        string Content;
    };

private:
    vector<FGuiText> Contents;
};

