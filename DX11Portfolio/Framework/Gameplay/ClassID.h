#pragma once

class UWorld;
class AActor;

class ClassID
{
public:
	using FactoryFunc = function<AActor* (UWorld*)>;

	// 클래스 등록
	static void Register(const string& name, FactoryFunc func);

	// 클래스 생성
	static AActor* Create(const string& name, UWorld* world);

	static bool Exists(const string& name);

	template<typename T>
	static T* Create(UWorld* world)
	{
		AActor* base = Create(T::StaticClassName(), world);
		return static_cast<T*>(base);
	}

private:
	// 1. 정적 초기화 순서 문제를 피하기 위해 함수 내부에 static 지역 변수를 사용했다.
	//    static 멤버 변수로 둘 경우, 그 멤버가 언제 초기화되는지는 컴파일러가 보장하지 않는다.
	//    일반적으로 static map 자료구조가 먼저 생성되고 REGISTER_CLASS 매크로가 실행되는 것이 의도된 흐름이지만,
	//    실제 실행 순서는 보장되지 않아, 매크로가 먼저 실행되고 자료구조가 나중에 초기화될 수도 있다.
	//    따라서 Register 호출 시점에 반드시 자료구조가 존재하도록, 
	//    함수 호출 순간에 한 번만 초기화되는 static 지역 변수를 사용했다.
	//
	// 2. cpp 파일에 정의할 수도 있지만, 그렇게 하면 cpp 파일 로드 시점에 초기화되어
	//    cpp 간의 로드 순서(translation unit 간 초기화 순서)에 의존하게 된다.
	//    반면 .h 파일 안의 함수 내부 static 변수는 ‘최초 호출 시점’에 초기화가 보장되어
	//    초기화 순서 문제를 완전히 방지할 수 있다.
	//
	// 3. 즉, 컴파일러가 코드를 ‘읽는 것’과 객체가 실제로 ‘초기화되는 시점’은 다르며,
	//    이 구조는 그 두 시점의 차이로 인한 불안정성을 제거하기 위한 lazy initialization 패턴이다.
	static unordered_map<string, FactoryFunc>& GetRegistry()
	{
		static std::unordered_map<std::string, FactoryFunc> registry;
        return registry;
	}
};