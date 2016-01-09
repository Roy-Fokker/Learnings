#pragma once

using namespace Learnings;

#pragma region Direct3D Device Wrapper

template<uint16_t SIZE>
GraphicsDevice::InputLayout GraphicsDevice::CreateInputLayout(const std::array<D3D11_INPUT_ELEMENT_DESC, SIZE>& elements, const std::vector<byte>& vsByteCode)
{
	InputLayout il;

	HRESULT hr = m_Device->CreateInputLayout(elements.data(), (uint32_t)elements.size(),
		vsByteCode.data(), (uint32_t)vsByteCode.size(),
		&il);
	ThrowIfFailed(hr, "Failed to create input layout");

	return il;
}

template<uint16_t SIZE>
bool GraphicsDevice::CheckInputLayout(const std::array<D3D11_INPUT_ELEMENT_DESC, SIZE>& elements, const std::vector<byte>& vsByteCode)
{
	HRESULT hr = m_Device->CreateInputLayout(elements.data(), (uint32_t)elements.size(),
		vsByteCode.data(), (uint32_t)vsByteCode.size(),
		NULL);
	// CreateInputLayout returns S_FALSE, 
	// if the signature of the InputLayout Matches that of the Shader's Inputs
	if (hr == S_FALSE)
	{
		return true;
	}

	return false;
}

#pragma endregion