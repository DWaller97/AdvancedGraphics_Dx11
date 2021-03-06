#include "ImGuiRenderer.h"

ImGuiRenderer* ImGuiRenderer::instance;
bool ImGuiRenderer::initialised = false;
HRESULT ImGuiRenderer::Init(HWND _hwnd, ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    // Application init: create a dear imgui context, setup some options, load fonts
    ImGui::CreateContext();
    // TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
    // TODO: Fill optional fields of the io structure later.
    // TODO: Load TTF/OTF fonts if you don't want to use the default font.

    // Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
    if (!ImGui_ImplWin32_Init(_hwnd) || !ImGui_ImplDX11_Init(_device, _deviceContext))
        return E_FAIL;
    if(!initialised)
        instance = new ImGuiRenderer();
    initialised = true;

    return S_OK;
}

ImGuiRenderer* ImGuiRenderer::GetInstance()
{
    if (!initialised) {
        std::cout << "ImGui is not initialised!" << std::endl;
        return nullptr;
    }
    return instance;
}

void ImGuiRenderer::Start()
{
    m_terrain = ObjectManager::GetTerrain();
    if (m_terrain == nullptr)
        return;
    m_terrainSeed = m_terrain->GetSeed();
    m_terrainSize = m_terrain->GetSize();
    m_terrainTextureHeights[0] = m_terrain->GetTextureHeight(0);
    m_terrainTextureHeights[1] = m_terrain->GetTextureHeight(1);
    m_terrainTextureHeights[2] = m_terrain->GetTextureHeight(2);
    m_terrainTextureHeights[3] = m_terrain->GetTextureHeight(3);
}

void ImGuiRenderer::Update(Light* _light)
{
    //io = ImGui::GetIO();
    Camera* cam = CameraManager::GetCurrCamera();
    camEye = cam->GetLook();
    camAt = cam->GetPosition();
    camUp = cam->GetUp();
    camSpeed = cam->GetMoveSpeed();
    _light->colour = XMFLOAT4(lightColour[0], lightColour[1], lightColour[2], lightColour[3]);
    _light->position = lightPosition;
    _light->rotation = lightRotation;
    if(m_terrain != nullptr)
        m_terrainPosition = m_terrain->GetPosition();
    DeferredRenderer::SetActive(deferred);

}

void ImGuiRenderer::Render()
{
    std::vector<GameObject*> _gameObjects = ObjectManager::GetObjects();
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    {
        ImGui::NewFrame();
        ImGui::Begin("Light");
        ImGui::Text("Colour");
        ImGui::Separator();
        ImGui::Indent();
        ImGui::ColorEdit3("Light Colour", lightColour);
        ImGui::NewLine();
        ImGui::Unindent();
        ImGui::Text("Positions");
        ImGui::Separator();
        ImGui::Indent();
        ImGui::SliderFloat("Position X", &lightPosition.x, -3, 3.0f);
        ImGui::SliderFloat("Position Y", &lightPosition.y, -3.0f, 3.0f);
        ImGui::SliderFloat("Position Z", &lightPosition.z, -3.0f, 3.0f);
        ImGui::Unindent();
        ImGui::Text("Rotations");
        ImGui::Separator();
        ImGui::Indent();
        ImGui::SliderFloat("Rotation X", &lightRotation.x, -1, 1.0f);
        ImGui::SliderFloat("Rotation Y", &lightRotation.y, -1.0f, 1.0f);
        ImGui::SliderFloat("Rotation Z", &lightRotation.z, -1.0f, 1.0f);
        ImGui::End();
    }
    {
        ImGui::Begin("Camera");
        ImGui::Text("Eye Position: %f %f %f", camEye.x, camEye.y, camEye.z);
        ImGui::Text("At Position: %f %f %f", camAt.x, camAt.y, camAt.z);
        ImGui::Text("Up Position: %f %f %f", camUp.x, camUp.y, camUp.z);
        ImGui::SliderFloat("Movement Speed", camSpeed, 0, 500);
        ImGui::End();
    }
    {
        ImGui::Begin("Objects");
        for (int i = 0; i < _gameObjects.size(); i++) {
            GameObject* currObj = _gameObjects.at(i);
            char c[] = "##";
            sprintf(c, "%d", i);
            char finalX[18] = "##X";
            strcat(finalX, c);
            char finalY[18] = "##Y";
            strcat(finalY, c);
            char finalZ[18] = "##Z";
            strcat(finalZ, c);
            ImGui::Separator();
            ImGui::Text("Object %d", i);
            ImGui::Indent();
            ImGui::Text("Position X");
            ImGui::SliderFloat(finalX, &currObj->m_position.x, -1500, 1500);
            ImGui::Text("Position Y");
            ImGui::SliderFloat(finalY, &currObj->m_position.y, -1500, 1500);
            ImGui::Text("Position Z");
            ImGui::SliderFloat(finalZ, &currObj->m_position.z, -1500, 1500);
            ImGui::Unindent();
        }
        ImGui::End();
        if (m_terrain != nullptr) {
            ImGui::Begin("Terrain");
            {
                ImGui::Text("Seed: %d", m_terrainSeed);
                ImGui::Text("Size: %d", m_terrainSize);
                ImGui::Text("Position: %f, %f, %f", m_terrainPosition.x, m_terrainPosition.y, m_terrainPosition.z);
                ImGui::SliderFloat("Texture Height 0", m_terrainTextureHeights[0], 0, 100, "%0.1");
                ImGui::SliderFloat("Texture Height 1", m_terrainTextureHeights[1], 100, 400, "%0.1");
                ImGui::SliderFloat("Texture Height 2", m_terrainTextureHeights[2], 400, 800, "%0.1");
                ImGui::SliderFloat("Texture Height 3", m_terrainTextureHeights[3], 800, 1500, "%0.1");
            }
            ImGui::End();
        }
        ImGui::Begin("Rendering");
        {
            ImGui::Checkbox("Deferred", &deferred);
            ImGui::Checkbox("Invert Screen Colours", &ObjectManager::GetRenderPlane()->pp_invertColours);
        }
        ImGui::End();

    }
    ImGui::Render();
}

void ImGuiRenderer::Cleanup()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
