#pragma once

enum class CpuZone : uint8_t
{
	GameThread = 0,
	RenderThread = 1,
	MAX
};

class PerfMon
{
public:
	static bool Init(ID3D11Device* device, int bufferedFrames = 3);

	// GPU 타이머
	static void BeginGpu(ID3D11DeviceContext* ctx);
	static void EndGpu(ID3D11DeviceContext* ctx);

	static void TryResolveGpu(ID3D11DeviceContext* ctx);

	// CPU 타이머 (각 스레드에서 자기 구간 감싸기)
	static void BeginCpu(CpuZone zone);
	static void EndCpu(CpuZone zone);
	
	// 최신 측정값(ms)
	static double GetGpuMs();                         // GPU 프레임 타임
	static double GetCpuMs(CpuZone zone);             // 게임/렌더 스레드 프레임 타임

	// 프레임 인덱스(디버그용)
	static int    GetFrameIndex();
	
private:
	struct GpuQueries
	{
		ComPtr<ID3D11Query> disjoint; // 해당 구간에서의 주파수(초당 틱 수), 해당 구간 타임스탬프가 유효한지
		ComPtr<ID3D11Query> start;    // 시작 GPU 틱 수 저장용
		ComPtr<ID3D11Query> end;      // 끝 GPU 틱 수 저장용
	};

	static bool CreateGpuQueriesForSlot(ID3D11Device* device, int slot);

private:
	// GPU
	static vector<GpuQueries> gpuFrames;					// present이후 바로 호출하는데
	static int      s_bufferedFrames;
	static int      frameIndex;
	static atomic<double> gpuMs;
	static bool     gpuInit;

	// CPU (QPC)
	static int64_t  cpuFreq;								// CPU에서 1초 틱 횟수
	static int64_t  cpuStart[(size_t)CpuZone::MAX];
	static atomic<double> cpuMs[(size_t)CpuZone::MAX];

};