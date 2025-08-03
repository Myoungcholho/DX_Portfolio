#include"Pch.h"
#include"Mirror.h"

//Mirror::Mirror(string name) 
//	: Name(name)
//{
//	auto mesh = GeomtryGenerator::MakeSquare(0.48f);
//	m_mirror = make_shared<Model>(vector<PBRMeshData>{mesh}, Name);
//}
//
//void Mirror::Initialize()
//{
//	m_mirror->Initialize();
//
//	m_mirror->m_basicPixelConstData.material.albedo = Vector3(0.3f);
//	m_mirror->m_basicPixelConstData.material.emission = Vector3(0.0f);
//	m_mirror->m_basicPixelConstData.material.metallic = 0.7f;
//	m_mirror->m_basicPixelConstData.material.roughness = 0.2f;
//
//	m_mirrorPlane = Plane(m_mirror->GetTransform()->GetPosition(), m_mirror->GetTransform()->GetForward());
//}
//
//void Mirror::Tick()
//{
//	m_mirror->Tick();
//	// 거울은 Light 따로?
//}
//
//void Mirror::UpdateConstantBuffer()
//{
//	m_mirrorPlane = Plane(m_mirror->GetTransform()->GetPosition(), m_mirror->GetTransform()->GetForward());
//	reflectionRow = Matrix::CreateReflection(m_mirrorPlane);
//	CContext::Get()->UpdateMirror(reflectionRow);
//}
//
//void Mirror::UpdateGUI()
//{
//	m_mirror->UpdateGUI();
//}
//
//void Mirror::Render()
//{
//	m_mirror->Render();
//}