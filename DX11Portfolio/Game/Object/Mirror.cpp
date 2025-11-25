#include"Pch.h"
#include"Mirror.h"

//Mirror::Mirror(string name) 
//	: name(name)
//{
//	auto mesh = GeomtryGenerator::MakeSquare(0.48f);
//	mirror = make_shared<Model>(vector<PBRMeshData>{mesh}, name);
//}
//
//void Mirror::Initialize()
//{
//	mirror->Initialize();
//
//	mirror->basicPixelConstData.material.albedo = Vector3(0.3f);
//	mirror->basicPixelConstData.material.emission = Vector3(0.0f);
//	mirror->basicPixelConstData.material.metallic = 0.7f;
//	mirror->basicPixelConstData.material.roughness = 0.2f;
//
//	mirrorPlane = Plane(mirror->GetTransform()->GetPosition(), mirror->GetTransform()->GetForward());
//}
//
//void Mirror::Tick()
//{
//	mirror->Tick();
//	// 거울은 Light 따로?
//}
//
//void Mirror::UpdateConstantBuffer()
//{
//	mirrorPlane = Plane(mirror->GetTransform()->GetPosition(), mirror->GetTransform()->GetForward());
//	reflectionRow = Matrix::CreateReflection(mirrorPlane);
//	CContext::Get()->UpdateMirror(reflectionRow);
//}
//
//void Mirror::UpdateGUI()
//{
//	mirror->UpdateGUI();
//}
//
//void Mirror::Render()
//{
//	mirror->Render();
//}