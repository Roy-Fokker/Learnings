#include "Utility.h"

#pragma region File Reader

#include <fstream>
std::vector<byte> ReadBinaryFile(const std::wstring &fileName)
{
	std::vector<byte> buffer;

	std::ifstream inFile(fileName, std::ios::in | std::ios::binary);

	if (!inFile.is_open())
	{
		throw std::runtime_error("Cannot open file");
	}

	buffer.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

	return buffer;
}

#pragma endregion
