#pragma once

#include <string>
#include <format>
#include <ostream>

struct HardwareData
{
	struct CPUData {
		std::string name;
		unsigned long long totalRAM; // bytes
		unsigned long long freeRAM; // bytes
		unsigned int cores;
		unsigned int physicalCores;
		double frequency; // GHz
	} cpuData;

	struct GPUData {
		std::string name;
		long vRAM; // bytes
	} gpuData;
};

static std::ostream& operator<<(std::ostream& os, const HardwareData::CPUData& data)
{
	os << "{\n";

	os << std::format("    {}: {},\n", "\"name\"", "\"" + data.name + "\"");
	os << std::format("    {}: {},\n", "\"cores\"", data.cores);
	os << std::format("    {}: {},\n", "\"physicalCores\"", data.physicalCores);
	os << std::format("    {}: {},\n", "\"freeRAM\"", data.freeRAM);
	os << std::format("    {}: {},\n", "\"totalRAM\"", data.totalRAM);
	os << std::format("    {}: {}\n",  "\"frequency\"", data.frequency);

	os << "}";

	return os;
}

static std::ostream& operator<<(std::ostream& os, const HardwareData::GPUData& data)
{
	os << "{\n";

	os << std::format("    {}: {},\n", "\"name\"", "\"" + data.name + "\"");
	os << std::format("    {}: {},\n", "\"vRAM\"", data.vRAM);

	os << "}";

	return os;
}


static std::ostream& operator<<(std::ostream& os, const HardwareData& data)
{
	os << "{\n";

	os << data.cpuData;
	os << ",";
	os << data.gpuData;

	os << "}";

	return os;
}


struct RuntimeData;

bool CollectHardwareData(HardwareData& data);

bool CollectRuntimeData(RuntimeData& data);
