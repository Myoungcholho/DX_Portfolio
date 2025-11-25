#include "Framework.h"
#include "BillboardData.h"

void BillboardInstance::Initialize(vector<BillboardPoint>& points)
{
	points = points;
	transform = make_shared<Transform>();

	// VertexBuffer 持失, 
	//D3D::Get()->CreateVertexBuffer(points, vertexBuffer);

	// VS, GS Cbuffer 持失
	WorldMatrixData.World = Matrix();
	//D3D::Get()->CreateConstantBuffer(WorldMatrixData, WorldConstantBuffer);

	TreeIndexData.index = rand() % 5;
	//D3D::Get()->CreateConstantBuffer(TreeIndexData, TreeIndexBuffer);
}

void BillboardInstance::Tick()
{
	WorldMatrixData.World = transform->GetWorldMatrix().Transpose();

	//D3D::Get()->UpdateBuffer(WorldMatrixData, WorldConstantBuffer);
}

void BillboardInstance::UpdateGUI()
{
	/*ImGui::Begin("Tree");
	{
		Vector3 pos = transform->GetPosition();
		if (ImGui::SliderFloat3("Position", &pos.x, -50.0f, 50.0f))
			transform->SetPosition(pos);

		Vector3 rot = transform->GetRotation();
		if (ImGui::SliderFloat3("Rotation", &rot.x, -180.0f, 180.0f))
			transform->SetRotation(rot);

		Vector3 scale = transform->GetScale();
		if (ImGui::SliderFloat3("Scale", &scale.x, 0.1f, 5.0f))
			transform->SetScale(scale);
	}
	ImGui::End();*/
}

void BillboardInstance::Render()
{
	D3D::Get()->GetDeviceContext()->VSSetConstantBuffers(2, 1, WorldConstantBuffer.GetAddressOf());
	D3D::Get()->GetDeviceContext()->GSSetConstantBuffers(2, 1, WorldConstantBuffer.GetAddressOf());
	D3D::Get()->GetDeviceContext()->PSSetConstantBuffers(0, 1, TreeIndexBuffer.GetAddressOf());
}

const std::vector<BillboardPoint>& BillboardInstance::GetPoints() const
{
	return points;
}

ComPtr<ID3D11Buffer> BillboardInstance::GetVertexBuffer()
{
	return vertexBuffer;
}

size_t BillboardInstance::GetPointCount()
{
	return points.size();
}
