#pragma once
#include <typeinfo>
#include <unordered_map>
#include <sys/stat.h>

class BaseLoader
{
public:
	BaseLoader(const BaseLoader& other) = delete;
	BaseLoader(BaseLoader&& other) noexcept = delete;
	BaseLoader& operator=(const BaseLoader& other) = delete;
	BaseLoader& operator=(BaseLoader&& other) noexcept = delete;
	BaseLoader() = default;
	virtual ~BaseLoader() = default;

	virtual const type_info& GetType() const = 0;
	virtual void Unload() = 0;
	virtual void SetDevice(ID3D11Device* pDevice) { m_pDevice = pDevice; }

protected:
	ID3D11Device* m_pDevice {nullptr};
};

template <class T>
class ContentLoader : public BaseLoader
{
	static std::unordered_map<std::wstring, T*> m_contentReferences;
	static int m_loaderReferences;

public:
	ContentLoader(const ContentLoader& other) = delete;
	ContentLoader(ContentLoader&& other) noexcept = delete;
	ContentLoader& operator=(const ContentLoader& other) = delete;
	ContentLoader& operator=(ContentLoader&& other) noexcept = delete;
	ContentLoader();
	virtual ~ContentLoader() = default;

	const type_info& GetType() const override { return typeid(T); }
	T* GetContent(const std::wstring& assetFile);
	void Unload() override;

protected:
	virtual T* LoadContent(const std::wstring& assetFile) = 0;
	virtual void Destroy(T* objToDestroy) = 0;
};

template <class T>
ContentLoader<T>::ContentLoader()
{
	++m_loaderReferences;
}

template <class T>
T* ContentLoader<T>::GetContent(const std::wstring& assetFile)
{
	for (std::pair<std::wstring, T*> kvp : m_contentReferences)
	{
		if (kvp.first.compare(assetFile) == 0)
			return kvp.second;
	}

	//Does File Exists?
	struct _stat buff;
	int result = -1;
	result = _wstat(assetFile.c_str(), &buff);
	if (result != 0)
	{
		std::wstringstream ss;
		ss << "ContentManager> File not found!\nPath: ";
		ss << assetFile;
		Logger::LogError(ss.str());
	}


	T* content = LoadContent(assetFile);
	if (content != nullptr)m_contentReferences.insert(std::pair<std::wstring, T*>(assetFile, content));

	return content;
}

#pragma warning(push)
#pragma warning(disable:4505)
template <class T>
void ContentLoader<T>::Unload()
{
	--m_loaderReferences;

	if (m_loaderReferences <= 0)
	{
		for (std::pair<std::wstring, T*> kvp : m_contentReferences)
		{
			Destroy(kvp.second);
		}

		m_contentReferences.clear();
	}
}
#pragma warning(pop)

template <class T>
std::unordered_map<std::wstring, T*> ContentLoader<T>::m_contentReferences = std::unordered_map<std::wstring, T*>();

template <class T>
int ContentLoader<T>::m_loaderReferences = 0;
