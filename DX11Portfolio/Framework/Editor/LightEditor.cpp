#include "Framework.h"
#include "LightEditor.h"

static ULightComponent* PickFirstLightComponent(AActor* actor)
{
	if (!actor)
		return nullptr;

	for (auto& comp : actor->GetComponents())
	{
		if (auto light = dynamic_pointer_cast<ULightComponent>(comp))
			return light.get();
	}

	return nullptr;
}

LightEditor::LightEditor(ULightComponent* target) : mTarget(target)
{
	RefreshBuffersFromTarget();
}

void LightEditor::SetTarget(ULightComponent* target)
{
	mTarget = target;
	mEditingAny = false;
	RefreshBuffersFromTarget();
}

void LightEditor::Update() {}

void LightEditor::OnGUI()
{
	if (!mTarget)
		return;

	// 편집 중이 아닐 때만 외부 변경을 버퍼에 반영
	if (!mEditingAny)
		RefreshBuffersFromTarget();

    ImGui::PushID(mTarget);              // ID 유니크화
    ImGui::SeparatorText("Light");       // 라벨은 동일해도 ID가 다르면 OK

    // Enabled
    bool v = enabled;
    if (ImGui::Checkbox("Enabled", &v)) {
        enabled = v;
        ApplyBuffersToTarget();
    }

    // Type
    const char* kTypes[] = { "Directional", "Point", "Spot" };
    int typeIdx = (type & LIGHT_DIRECTIONAL) ? 0 :
                  (type & LIGHT_POINT) ? 1 :
                  (type & LIGHT_SPOT) ? 2 : 0;

    if (ImGui::Combo("Type", &typeIdx, kTypes, IM_ARRAYSIZE(kTypes))) {
        uint32_t baseType = 0;
        switch (typeIdx) {
        case 0: baseType = LIGHT_DIRECTIONAL; break;
        case 1: baseType = LIGHT_POINT;       break;
        case 2: baseType = LIGHT_SPOT;        break;
        }

        uint32_t shadowFlag = (type & LIGHT_SHADOW);

        type = baseType | shadowFlag;
        ApplyBuffersToTarget();
    }

    // Radiance
    Vector3 c = radiance;
    if (ImGui::ColorEdit3("Radiance", &c.x)) {
        radiance = c;
        ApplyBuffersToTarget();
    }

    // Falloff
    float start = fallOffStart, end = fallOffEnd;
    bool changed = false;
    changed |= ImGui::DragFloat("Falloff Start", &start, 0.01f, 0.0f, FLT_MAX, "%.3f");
    changed |= ImGui::DragFloat("Falloff End", &end, 0.01f, 0.0f, FLT_MAX, "%.3f");
    if (changed) {
        if (end < start) end = start;
        fallOffStart = (std::max)(0.0f, start);
        fallOffEnd = (std::max)(fallOffStart, end);
        ApplyBuffersToTarget();
    }

    // Spot Power
    float p = spotPower;
    if (ImGui::DragFloat("Spot Power", &p, 0.1f, 0.1f, 1024.0f, "%.2f")) {
        spotPower = (std::max)(0.1f, p);
        ApplyBuffersToTarget();
    }

    ImGui::PopID();
	 
}

// 타겟의 값을 UI버퍼에 저장
void LightEditor::RefreshBuffersFromTarget()
{
	if (mTarget == nullptr)
		return;

	radiance = mTarget->GetRadiance();
	fallOffStart = mTarget->GetFalloffStart();
	fallOffEnd = mTarget->GetFalloffEnd();
	spotPower = mTarget->GetSpotPower();
	type = mTarget->GetLightType();

}

// UI의 값을 타겟에 반영
void LightEditor::ApplyBuffersToTarget()
{
	if (mTarget == nullptr)
		return;

	mTarget->SetLightType((uint32_t)type);
	mTarget->SetRadiance(radiance);
	mTarget->SetFalloff(fallOffStart, fallOffEnd);
	mTarget->SetSpotPower(spotPower);
	mTarget->SetEnabled(enabled);

}