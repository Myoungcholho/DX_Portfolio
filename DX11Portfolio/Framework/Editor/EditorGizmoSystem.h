#pragma once

enum class GizmoOp{Translate, Rotate, Scale};
enum class GizmoSpace { Local, World };

struct GizmoConfig
{
	GizmoOp op = GizmoOp::Translate;
	GizmoSpace space = GizmoSpace::Local;
	bool snap = false;
	float snapTranslate = 0.1f;
	float snapRotateDeg = 5.0f;
	float snapScale = 0.1f;
};

class EditorGizmoSystem
{
public:
	static GizmoConfig& config();
	static void Draw(const Matrix& view, const Matrix& proj, ImVec2 viewportPos, ImVec2 viewportSize);
};