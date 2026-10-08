#include "stdafx.h"

#include "HardwareData.h"

#include <intrin.h>
#include <sysinfoapi.h>
#include <thread>
#include <windows.h>
#include <vector>
#include <ostream>

static bool CollectDataGPU(HardwareData& data)
{
	ID3D11Device* device = DeviceManager::Device();
	if (!device)
		return false;

	HRESULT hr;
	IDXGIDevice* dxgiDevice = nullptr;

	hr = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
		return false;

	IDXGIAdapter* adapter = nullptr;
	hr = dxgiDevice->GetAdapter(&adapter);
	if (FAILED(hr))
		return false;

	DXGI_ADAPTER_DESC desc;
	hr = adapter->GetDesc(&desc);
	if (FAILED(hr))
		return false;

	std::wstring gpuName = std::wstring(desc.Description);
	data.gpuData.name = std::string(gpuName.begin(), gpuName.end());

	data.gpuData.vRAM = desc.DedicatedVideoMemory;

	ReleaseCOM(adapter);
	ReleaseCOM(dxgiDevice);

	return true;
}

static std::string GetNameCPU()
{
	int cpuInfo[4] = { -1 };
    char brand[0x40] = {};

    __cpuid(cpuInfo, 0x80000000);
    unsigned int nExIds = cpuInfo[0];

    if (nExIds >= 0x80000004)
    {
        __cpuid((int*)cpuInfo, 0x80000002);
        memcpy(brand, cpuInfo, sizeof(cpuInfo));

        __cpuid((int*)cpuInfo, 0x80000003);
        memcpy(brand + 16, cpuInfo, sizeof(cpuInfo));

        __cpuid((int*)cpuInfo, 0x80000004);
        memcpy(brand + 32, cpuInfo, sizeof(cpuInfo));
    }

    return std::string(brand);
}

static unsigned int GetCoresCPU()
{
	return std::thread::hardware_concurrency();
}

static unsigned int GetPhysicalCoresCPU()
{
	DWORD len = 0;
    GetLogicalProcessorInformation(nullptr, &len);

    std::vector<uint8_t> buffer(len);
    auto info = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)buffer.data();

    GetLogicalProcessorInformation(info, &len);

    int coreCount = 0;

    for (DWORD i = 0; i < len / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)
    {
        if (info[i].Relationship == RelationProcessorCore)
            coreCount++;
    }

    return coreCount;
}

static double GetFrequencyCPU()
{
	LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return (double)freq.QuadPart / 1e9;
}

static bool CollectDataCPU(HardwareData& data)
{
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	if (!GlobalMemoryStatusEx(&memStatus))
		return false;

	data.cpuData.name = GetNameCPU();
	data.cpuData.cores = GetCoresCPU();
	data.cpuData.physicalCores = GetPhysicalCoresCPU();
	data.cpuData.totalRAM = memStatus.ullTotalPhys;
	data.cpuData.freeRAM = memStatus.ullAvailPhys;
	data.cpuData.frequency = GetFrequencyCPU();

	return true;
}

bool CollectHardwareData(HardwareData& data)
{
	if (!CollectDataGPU(data))
		return false;

	if (!CollectDataCPU(data))
		return false;

	return true;
}

bool CollectRuntimeData(RuntimeData& data)
{
	return false;
}
