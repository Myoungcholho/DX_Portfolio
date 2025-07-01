#pragma once

class FMath
{
public:
	static int Random(int r1, int r2);
	static float Random(float r1, float r2);
    static DirectX::SimpleMath::Matrix CreateLookAtLH(
        const DirectX::SimpleMath::Vector3& eye,
        const DirectX::SimpleMath::Vector3& target,
        const DirectX::SimpleMath::Vector3& up
    );
};