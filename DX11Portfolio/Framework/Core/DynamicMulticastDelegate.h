#pragma once

#include <functional>
#include <algorithm>

struct FDelegateHandle
{
    uint64_t Id = 0;
    explicit FDelegateHandle(uint64_t InId = 0) : Id(InId) {}
    bool IsValid() const { return Id != 0; }
    bool operator==(const FDelegateHandle& other) const { return Id == other.Id; }
};

struct FBoundFunction
{
    void* ObjectPtr = nullptr;          // 함수가지는 객체
    std::function<void()> Invoker;      // 실행할 함수 주소
    FDelegateHandle Handle;             // 디버깅용 핸들
    std::string FunctionName;           // 함수 이름 Delete용

    bool Matches(void* InObj, const std::string& InFuncName) const
    {
        return InObj == ObjectPtr && FunctionName == InFuncName;
    }
};

class FDynamicMulticastDelegate
{
public:
    template<typename T>
    FDelegateHandle AddDynamic(T* Object, void (T::* Method)(), const std::string& MethodName)
    {
        FBoundFunction Bound;
        Bound.ObjectPtr = static_cast<void*>(Object);
        Bound.Invoker = [Object, Method]() { (Object->*Method)(); };
        Bound.Handle = FDelegateHandle(NextId++);
        Bound.FunctionName = MethodName;
        Delegates.push_back(std::move(Bound));
        return Bound.Handle;
    }

    template<typename T>
    void RemoveDynamic(T* Object, const std::string& MethodName)
    {
        Delegates.erase(std::remove_if(Delegates.begin(), Delegates.end(),
            [Object, &MethodName](const FBoundFunction& bound)
            {
                return bound.Matches(static_cast<void*>(Object), MethodName);
            }), Delegates.end());
    }
    void Remove(FDelegateHandle handle);

    void Broadcast();
    void Clear();
    bool IsBound() const;

private:
    std::vector<FBoundFunction> Delegates;
    uint64_t NextId = 1;
};