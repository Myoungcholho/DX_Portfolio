#include "Framework.h"
#include "Texture.h"

CTexture::CTexture(string InFileName, bool bDefaultPath)
	:CTexture("", CString::ToWString(InFileName), bDefaultPath)
{
	// 생성자 호출 위임
}

CTexture::CTexture(wstring InFileName, bool bDefaultPath)
	: CTexture("", InFileName, bDefaultPath)
{
	// InParamName 없이 호출한 경우 빈 문자열로
}

CTexture::CTexture(string InParamName, wstring InFileName, bool bDefaultPath)
{
	FileName = InFileName;

	if (bDefaultPath)
		FileName = L"../../../_Textures/" + InFileName;
	else
		FileName = InFileName;

	Metadata = GetMetaData(FileName);
	SRV = CreateSRV(FileName, Metadata);

	FileName = CPath::GetFileNameWithoutExtension(InFileName);
}

CTexture::~CTexture()
{

}


DirectX::TexMetadata CTexture::GetMetaData(wstring InFilePath)
{
	HRESULT hr;
	DirectX::TexMetadata metadata;

	wstring ext = CPath::GetExtension(InFilePath);
	if (ext == L"tga")
	{
		hr = GetMetadataFromTGAFile(InFilePath.c_str(), metadata);
		assert((hr) >= 0);
	}
	else if (ext == L"dds")
	{
		hr = GetMetadataFromDDSFile(InFilePath.c_str(), DDS_FLAGS_NONE, metadata);
		assert((hr) >= 0);
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = GetMetadataFromWICFile(InFilePath.c_str(), WIC_FLAGS_NONE, metadata);
		assert((hr) >= 0);
	}

	return metadata;
}

ID3D11ShaderResourceView* CTexture::CreateSRV(wstring InFilePath, DirectX::TexMetadata& InMetadata)
{
	HRESULT hr;

	ScratchImage image;

	wstring ext = CPath::GetExtension(InFilePath);
	if (ext == L"tga")
	{
		hr = LoadFromTGAFile(InFilePath.c_str(), &InMetadata, image);
		assert((hr) >= 0);
	}
	else if (ext == L"dds")
	{
		hr = LoadFromDDSFile(InFilePath.c_str(), DDS_FLAGS_NONE, &InMetadata, image);
		assert((hr) >= 0);
	}
	else if (ext == L"hdr")
	{
		assert(false);
	}
	else
	{
		hr = LoadFromWICFile(InFilePath.c_str(), WIC_FLAGS_NONE, &InMetadata, image);
		assert((hr) >= 0);
	}


	ID3D11ShaderResourceView* srv = nullptr;
	hr = DirectX::CreateShaderResourceView(D3D::Get()->GetDevice(), image.GetImages(), image.GetImageCount(), InMetadata ,&srv);
	assert((hr) >= 0);

	return srv;
}
