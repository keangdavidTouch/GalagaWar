
#include "stdafx.h"

#include "BinaryReader.h"


BinaryReader::BinaryReader(void):
	m_Exists(false),
	m_pReader(nullptr)
{
}

BinaryReader::~BinaryReader(void)
{
	Close();
}

std::wstring BinaryReader::ReadLongString()
{
	if(m_pReader==nullptr)
		Logger::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	auto stringLength = Read<UINT>();
	
 std::wstringstream ss;
	for(UINT i=0; i<stringLength; ++i)
	{
		ss<<Read<wchar_t>();
	}

	return (std::wstring)ss.str();
}

std::wstring BinaryReader::ReadNullString()
{
	if(m_pReader==nullptr)
	{
		Logger::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");
		return L"";
	}

 std::string buff;
	getline(*m_pReader, buff, '\0');

	return std::wstring(buff.begin(),buff.end());
}


std::wstring BinaryReader::ReadString()
{
	if(m_pReader==nullptr)
		Logger::LogError(L"BinaryReader doesn't exist!\nUnable to read binary data...");

	int stringLength = (int)Read<char>();
	
 std::wstringstream ss;
	for(int i=0; i<stringLength; ++i)
	{
		ss<<Read<char>();
	}

	return (std::wstring)ss.str();
}

void BinaryReader::Open(std::wstring binaryFile)
{
	Close();

	auto temp = new std::ifstream();
	temp->open(binaryFile, std::ios::in| std::ios::binary);
	if(temp->is_open())
	{
		m_pReader = temp;
		m_Exists = true;
	}
	else
	{
	 std::wstringstream ss;
		ss<<L"BinaryReader::Open> Failed to open the file!\nBinaryFile: ";
		ss<<binaryFile;
		Logger::LogWarning(ss.str());
		Close();
	}
}

void BinaryReader::Open(char* s, UINT32 size)
{
	Close();

 std::string data(s, size);
	m_pReader = new std::istringstream(data);
	m_Exists = true;
}

void BinaryReader::Close()
{
	SafeDelete(m_pReader);
	m_Exists = false;
}

int BinaryReader::GetBufferPosition()
{
	if(m_pReader)
	{
		return static_cast<int>(m_pReader->tellg());
	}

	Logger::LogWarning(L"BinaryReader::GetBufferPosition> m_pReader doesn't exist");
	return -1;
}

bool BinaryReader::SetBufferPosition(int pos)
{
	if(m_pReader)
	{
		m_pReader->seekg(pos);
		return true;
	}

	Logger::LogWarning(L"BinaryReader::SetBufferPosition> m_pReader doesn't exist");
	return false;
}

bool BinaryReader::MoveBufferPosition(int move)
{
	auto currPos = GetBufferPosition();
	if(currPos>0)
	{
		return SetBufferPosition(currPos + move);
	}

	return false;
}
