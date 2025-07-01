#include "Framework.h"
#include "Math.h"

int FMath::Random(int r1, int r2)
{
	return (int)(rand() % (r2 - r1 + 1)) + r1;
}

float FMath::Random(float r1, float r2)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = r2 - r1;
	float val = random * diff;

	return r1 + val;
}


Matrix FMath::CreateLookAtLH(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    XMVECTOR eyeVec = XMLoadFloat3(&eye);
    XMVECTOR targetVec = XMLoadFloat3(&target);
    XMVECTOR upVec = XMLoadFloat3(&up);

    XMMATRIX viewXM = XMMatrixLookAtLH(eyeVec, targetVec, upVec);

    Matrix result;
    XMStoreFloat4x4(&result, viewXM);

    return result;
}
