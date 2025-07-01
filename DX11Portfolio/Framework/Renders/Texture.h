#pragma once

class CTexture
{
public:
	CTexture(string InFileName, bool bDefaultPath = true);
	CTexture(wstring InFileName, bool bDefaultPath = true);
	CTexture(string InParamName, wstring InFileName, bool bDefaultPath = true);
	~CTexture();

public:
	ID3D11ShaderResourceView* GetSRV() { return SRV; }
public:
	UINT GetWidth() { return Metadata.width; }
	UINT GetHeight() { return Metadata.height; }

public:
	DirectX::TexMetadata GetMetaData(wstring InFilePath);
	ID3D11ShaderResourceView* CreateSRV(wstring InFilePath, DirectX::TexMetadata& InMetadata);

private:
	ID3D11ShaderResourceView* SRV = nullptr;
	DirectX::TexMetadata Metadata;
private:
	wstring FileName;
};