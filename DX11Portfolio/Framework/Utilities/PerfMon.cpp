#include "Framework.h"
#include "PerfMon.h"

vector<PerfMon::GpuQueries> PerfMon::gpuFrames;
int PerfMon::s_bufferedFrames = 0;
int PerfMon::frameIndex = 0;
atomic<double> PerfMon::gpuMs{ 0.0 };
bool PerfMon::gpuInit = false;

int64_t PerfMon::cpuFreq = 0;
int64_t PerfMon::cpuStart[(size_t)CpuZone::MAX] = { 0,0 };
atomic<double> PerfMon::cpuMs[(size_t)CpuZone::MAX] = { 0.0,0.0 };

static inline int64_t QpcNow()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);           // 시간 틱 반환
    return li.QuadPart;
}
static inline int64_t QpcFreq()
{
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);         // 1초에 몇번 틱이 호출되는지 반환
    return li.QuadPart;
}

bool PerfMon::Init(ID3D11Device* device, int bufferedFrames)
{
    if (!device)
        return false;

    // CPU
    cpuFreq = QpcFreq();

    // GPU
    s_bufferedFrames = bufferedFrames > 1 ? bufferedFrames : 3;
    gpuFrames.resize(s_bufferedFrames);

    for (int i = 0; i < s_bufferedFrames; ++i)
    {
        if (CreateGpuQueriesForSlot(device, i) == false)
            return false;
    }

    frameIndex = 0;
    gpuMs.store(0.0);
    cpuMs[(size_t)CpuZone::GameThread].store(0.0);
    cpuMs[(size_t)CpuZone::RenderThread].store(0.0);
    gpuInit = true;

    return true;
}

void PerfMon::BeginGpu(ID3D11DeviceContext* ctx)
{
    if (!gpuInit || !ctx) return;

    const int curr = frameIndex % s_bufferedFrames;

    ctx->Begin(gpuFrames[curr].disjoint.Get());
    ctx->End(gpuFrames[curr].start.Get());

}

void PerfMon::EndGpu(ID3D11DeviceContext* ctx)
{
    if (!gpuInit || !ctx) return;

    const int curr = frameIndex % s_bufferedFrames;

    ctx->End(gpuFrames[curr].end.Get());
    ctx->End(gpuFrames[curr].disjoint.Get());

    // 프레임 증가
    ++frameIndex;
}

void PerfMon::TryResolveGpu(ID3D11DeviceContext* ctx)
{
    if (!gpuInit || !ctx) return;
    if (frameIndex < 2) return;

    // 2~3프레임 지연을 권장 (bufferedFrames 한도 내)
    const int resolveLag = std::min(3, s_bufferedFrames - 1);

    for (int back = resolveLag; back >= 1; --back)
    {
        const int slot = (frameIndex - back + s_bufferedFrames * 8) % s_bufferedFrames;

        D3D11_QUERY_DATA_TIMESTAMP_DISJOINT dj = {};

        if (ctx->GetData(gpuFrames[slot].disjoint.Get(), &dj, sizeof(dj),
            D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK || dj.Disjoint)
            continue;

        UINT64 s = 0, e = 0;
        if (ctx->GetData(gpuFrames[slot].start.Get(), &s, sizeof(s), D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK) continue;
        if (ctx->GetData(gpuFrames[slot].end.Get(), &e, sizeof(e), D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK) continue;

        if (e > s)
        {
            const double ms = double(e - s) / double(dj.Frequency) * 1000.0;
            gpuMs.store(ms);
            return; // 준비된 슬롯 하나만 갱신하고 종료
        }
    }
}

void PerfMon::BeginCpu(CpuZone zone)
{
    size_t z = (size_t)zone;
    cpuStart[z] = QpcNow();
}

void PerfMon::EndCpu(CpuZone zone)
{
    size_t z = (size_t)zone;
    int64_t end = QpcNow();
    cpuMs[z].store(((end - cpuStart[z]) * 1000.0) / double(cpuFreq));
}

double PerfMon::GetGpuMs()
{
    return gpuMs.load();
}

double PerfMon::GetCpuMs(CpuZone zone)
{
    return cpuMs[(size_t)zone].load();
}

int PerfMon::GetFrameIndex()
{
    return frameIndex;
}

bool PerfMon::CreateGpuQueriesForSlot(ID3D11Device* device, int slot)
{
    D3D11_QUERY_DESC qd = {};

    qd.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
    device->CreateQuery(&qd, gpuFrames[slot].disjoint.ReleaseAndGetAddressOf());
    

    qd.Query = D3D11_QUERY_TIMESTAMP;
    device->CreateQuery(&qd, gpuFrames[slot].start.ReleaseAndGetAddressOf());

    qd.Query = D3D11_QUERY_TIMESTAMP;
    device->CreateQuery(&qd, gpuFrames[slot].end.ReleaseAndGetAddressOf());

    return true;
}
