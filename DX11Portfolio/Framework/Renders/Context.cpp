#include "Framework.h"
#include "Context.h"
#include <format>

CContext* CContext::Instance = nullptr;

void CContext::Create()
{
	assert(Instance == nullptr);

	Instance = new CContext();
}

void CContext::Destroy()
{
	assert(Instance != nullptr);

	delete Instance;
	Instance = nullptr;
}

CContext* CContext::Get()
{
	return Instance;
}

CContext::CContext()
{
	View = std::make_unique<CCamera>();

	float width = D3D::GetDesc().Width;
	float height = D3D::GetDesc().Height;

	Viewport = std::make_unique<D3D11_VIEWPORT>();
	Viewport->TopLeftX = 0;
	Viewport->TopLeftY = 0;
	Viewport->Width = width;
	Viewport->Height = height;
	Viewport->MinDepth = 0;
	Viewport->MaxDepth = 1;

	//m_constantBufferData.viewProj = Matrix();
	//D3D::Get()->CreateConstantBuffer(m_constantBufferData, m_constantBuffer);
	//D3D11Utils::CreateConstBuffer(D3D::Get()->GetDeviceCom(), m_constantBufferData, m_constantBuffer);

	//mirrorViewProjectionConstantBufferData.mirrorViewProj = Matrix();
	//D3D::Get()->CreateConstantBuffer(mirrorViewProjectionConstantBufferData, mirrorViewProjectionConstantBuffer);
	
	//D3D11Utils::CreateConstBuffer(D3D::Get()->GetDeviceCom(), mirrorViewProjectionConstantBufferData, mirrorViewProjectionConstantBuffer);
}

CContext::~CContext()
{
	
}

void CContext::Tick()
{
	Tick_View();
	Tick_WorldTime();
}

void CContext::Tick_View()
{
	if (CMouse::Get()->Press(EMouseButton::Right))
	{
		Vector3 Position = View->GetPosition();
		if (CKeyboard::Get()->Press('W'))
			Position = Position + View->GetForward() * View->GetMoveSpeed() * CTimer::Get()->GetDeltaTime();
		else if (CKeyboard::Get()->Press('S'))
			Position = Position - View->GetForward() * View->GetMoveSpeed() * CTimer::Get()->GetDeltaTime();

		if (CKeyboard::Get()->Press('D'))
			Position = Position + View->GetRight() * View->GetMoveSpeed() * CTimer::Get()->GetDeltaTime();
		else if (CKeyboard::Get()->Press('A'))
			Position = Position - View->GetRight() * View->GetMoveSpeed() * CTimer::Get()->GetDeltaTime();

		if (CKeyboard::Get()->Press('E'))
			Position = Position + View->GetUp() * View->GetMoveSpeed() * CTimer::Get()->GetDeltaTime();
		else if (CKeyboard::Get()->Press('Q'))
			Position = Position - View->GetUp() * View->GetMoveSpeed() * CTimer::Get()->GetDeltaTime();

		View->SetPosition(Position);


		Vector3 delta = CMouse::Get()->GetMoveDelta();

		Vector3 Rotation = View->GetRotation();
		Rotation.x = Rotation.x + delta.y * View->GetRotationSpeed() * CTimer::Get()->GetDeltaTime();
		Rotation.y = Rotation.y + delta.x * View->GetRotationSpeed() * CTimer::Get()->GetDeltaTime();
		Rotation.z = 0.0f;

		View->SetRotation(Rotation);
	}

	View->Tick();
}

void CContext::Tick_WorldTime()
{
	if (CKeyboard::Get()->Press(VK_ADD))
		CTimer::Get()->AddWorldTime(*CTimer::Get()->GetManualTimeSpeed());

	if (CKeyboard::Get()->Press(VK_SUBTRACT))
		CTimer::Get()->AddWorldTime(*CTimer::Get()->GetManualTimeSpeed() * -1.0f);
	
}

void CContext::UpdateMirror(const Matrix& mirror)
{
	//this->mirror = mirror;

	//Matrix view = GetViewMatrix();
	//Matrix proj = GetProjectionMatrix();
	//mirrorViewProjectionConstantBufferData.mirrorViewProj = (mirror * view * proj).Transpose();

	//D3D::Get()->UpdateBuffer(mirrorViewProjectionConstantBufferData, mirrorViewProjectionConstantBuffer);
	//D3D11Utils::UpdateBuffer(D3D::Get()->GetDeviceCom(), D3D::Get()->GetDeviceContextCom(),mirrorViewProjectionConstantBufferData, mirrorViewProjectionConstantBuffer);
}

void CContext::Render()
{
	// 카메라와 관련된 설정으로 Context가 하는게 맞다.
	D3D::Get()->GetDeviceContext()->RSSetViewports(1, Viewport.get());

	string str = string("FrameRate : ") + to_string((int)ImGui::GetIO().Framerate);
	ImGuiManager::Get()->RenderText(5, 5, 1, 1, 1, str);

	str = std::format("Running Time : {:0.6f}", CTimer::Get()->GetRunningTime());
	ImGuiManager::Get()->RenderText(5, 20, 1, 1, 1, str);


	Vector3 rotation = View->GetRotation();
	Vector3 position = View->GetPosition();

	str = std::format("View Rotation : {:3.0f}, {:3.0f}, {:3.0f}", rotation.x, rotation.y, rotation.z);
	ImGuiManager::Get()->RenderText(5, 35, 1, 1, 1, str);

	str = std::format("View Position : {:3.2f}, {:3.2f}, {:3.2f}", position.x, position.y, position.z);
	ImGuiManager::Get()->RenderText(5, 50, 1, 1, 1, str);
}

void CContext::RenderConstantBuffer()
{
	// View Projection ConstantBuffer로 전송
	//D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
	//D3D::Get()->GetDeviceContext()->GSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
	//D3D::Get()->GetDeviceContext()->HSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
	//D3D::Get()->GetDeviceContext()->DSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
}

void CContext::RenderMirrorConstantBuffer()
{
	// Mirror View Projection 전송
	//D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(1, 1, mirrorViewProjectionConstantBuffer.GetAddressOf());
}

void CContext::ResizeScreen()
{
	float width = D3D::GetDesc().Width;
	float height = D3D::GetDesc().Height;

	Viewport->TopLeftX = 0;
	Viewport->TopLeftY = 0;
	Viewport->Width = width;
	Viewport->Height = height;
	Viewport->MinDepth = 0;
	Viewport->MaxDepth = 1;
}

SimpleMath::Matrix CContext::GetViewMatrix()
{
	return View->GetViewMatrix();
}

SimpleMath::Matrix CContext::GetProjectionMatrix()
{
	return View->GetProjectionMatrix();
}

//SimpleMath::Matrix CContext::GetMirrorMatrix()
//{
//	//return mirror;
//}
