#include "Framework.h"
#include "URenderProxy.h"

URenderProxy::URenderProxy()
{
	device = D3D::Get()->GetDeviceCom();
	context = D3D::Get()->GetDeviceContextCom();
}
