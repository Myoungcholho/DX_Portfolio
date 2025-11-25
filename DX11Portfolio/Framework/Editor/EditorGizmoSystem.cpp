#include "Framework.h"
#include "EditorGizmoSystem.h"

static GizmoConfig g_gizmoConfig;

GizmoConfig& EditorGizmoSystem::config()
{
	return g_gizmoConfig;
}

void EditorGizmoSystem::Draw(const Matrix& view, const Matrix& proj, ImVec2 viewportPos, ImVec2 viewportSize)
{
	UActorComponent* comp = EditorSelection::GetActorComponent();
	if (comp == nullptr)
		return;

	USceneComponent* sc = dynamic_cast<USceneComponent*>(comp);
	if (sc == nullptr)
		return;

	Matrix target = sc->GetWorldMatrix();			// worldTransform의 worldMatrix 행렬 반환.

	ImGuizmo::BeginFrame();
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
	ImGuizmo::SetOrthographic(false);

	// -- Mode Mapping, 기본은 Translate..
	ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
	if (g_gizmoConfig.op == GizmoOp::Rotate) op = ImGuizmo::ROTATE;
	if (g_gizmoConfig.op == GizmoOp::Scale) op = ImGuizmo::SCALE;

	ImGuizmo::MODE mode = (g_gizmoConfig.space == GizmoSpace::Local) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

	float* pView = (float*)&view;
	float* pProj = (float*)&proj;
	float* pTarget = (float*)&target;

	bool used = false;
	if (g_gizmoConfig.snap) 
	{
		float snap[3];
		if (op == ImGuizmo::TRANSLATE)
			snap[0] = snap[1] = snap[2] = g_gizmoConfig.snapTranslate;
		else if (op == ImGuizmo::ROTATE)
			snap[0] = snap[1] = snap[2] = g_gizmoConfig.snapRotateDeg;
		else
			snap[0] = snap[1] = snap[2] = g_gizmoConfig.snapScale;

		// Manipulate(view행렬, proj행렬, 기즈모타입, 로컬/월드, in,out 대상 , 이번 프레임 변환분(nullptr), 스냅값)
		used = ImGuizmo::Manipulate(pView, pProj, op, mode, pTarget, nullptr, snap);
	}
	else {
		used = ImGuizmo::Manipulate(pView, pProj, op, mode, pTarget, nullptr, nullptr);
	}

	// ImGuizmo가 변환하는 것은 WorldTransform이다. 그래서 부모가 있다면 Local에 반영을 해줘야하고
	// 부모의 역행렬을 취해 부모값을 지운뒤 행렬 분해로 Local에 적용하는 것이 좋다.
	if (used)
	{
		Matrix targetW = target;

		Matrix newLocalM;
		if (USceneComponent* parent = sc->GetParent())
		{
			Matrix parentW = parent->GetWorldMatrix();
			newLocalM = parentW.Invert() * targetW;
		}
		else
		{
			newLocalM = targetW;
		}

		// 여기서 분해해서 적용
		Transform newLocalT;
		newLocalT.SetWorldMatrix(newLocalM);
		sc->SetRelativeTransform(newLocalT);

	}

}