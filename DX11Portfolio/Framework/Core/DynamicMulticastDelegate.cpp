#include "Framework.h"
#include "DynamicMulticastDelegate.h"

void FDynamicMulticastDelegate::Broadcast()
{
	for (const auto& D : Delegates)
		D.Invoker();
}

void FDynamicMulticastDelegate::Clear()
{
	Delegates.clear();
}

bool FDynamicMulticastDelegate::IsBound() const
{
	return !Delegates.empty();
}

void FDynamicMulticastDelegate::Remove(FDelegateHandle handle)
{
	Delegates.erase(std::remove_if(Delegates.begin(), Delegates.end(),
		[&](const FBoundFunction& b) { return b.Handle == handle; }),
		Delegates.end());
}
