#pragma once

#include "define.h"
#include <windows.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <assert.h>


// ��
struct CObjectLock
{
	CObjectLock(::CRITICAL_SECTION*cs) :pcs(cs) { ::EnterCriticalSection(pcs); }
	~CObjectLock() { ::LeaveCriticalSection(pcs); }
	::CRITICAL_SECTION* pcs;
};


// ����ػ���ӿڣ��ṩ��gc�����ռ�������
class IObjectPool
{
public:
	// �����ռ�
	virtual void GC(bool bEnforce) = 0;
	virtual ~IObjectPool() {}
};

// ����ص�ʵ���࣬ͨ��ģ��ʵ��ͨ�û�
template<class ObjectType>
class CObjectPool : public IObjectPool
{
private:
	typedef std::unordered_map<int, char*>  FreePointer;
	typedef std::vector<char*>   FreeIndex;
	typedef void* (*ObjectMalloc)(size_t);
	typedef void (*ObjectFree)(void*);
public:
	CObjectPool()
	{
		::InitializeCriticalSection(&m_cs);
		CObjectLock lock(&m_cs);
		m_growSize = 64;
		m_MaxObject = m_growSize * 3;
		m_CurrentObject = 0;
	}

	~CObjectPool()
	{
		GC(true);
		::DeleteCriticalSection(&m_cs);
	}

	//����max�����ֵ,����ǳɳ������ı�����ϵ,��64 128 256 512 1024,
	void SetMaxObject(uint32 uMax)
	{
		m_MaxObject = uMax;
	}

	uint32 GetMaxObjects()
	{
		return m_MaxObject;
	}

	uint32 GetCurrentObjects()
	{
		return m_CurrentObject;
	}

	//�޲ι���
	ObjectType* Construct()
	{
		CObjectLock lock(&m_cs);

		char* pData = GetFreePointer();
		if (pData == nullptr)
		{
			//��¼��־
			OUR_DEBUG((LM_ERROR, "[CObjectPool::Construct()] pData == nullptr (%s).(%d)\n", __FILE__, __LINE__));
			assert(pData);
			return nullptr;
		}
			

		ObjectType * const ret = new (pData)ObjectType();

		return ret;
	}

	//���ι���
	template<class ... Args>
	ObjectType* Construct(Args && ... args)
	{
		CObjectLock lock(&m_cs);

		char* pData = GetFreePointer();
		if (pData == nullptr)
		{
			//��¼��־
			OUR_DEBUG((LM_ERROR, "[CObjectPool::Construct(Args && ... args)] pData == nullptr (%s).(%d)\n", __FILE__, __LINE__));
			assert(pData);
			assert(pData);
			return nullptr;
		}
			

		//����ģ��Ĺ��캯��
		ObjectType* const ret = new (pData) ObjectType(std::forward<Args>(args)...);

		return ret;
	}

	// ����һ������
	void Destroy(ObjectType* const object)
	{
		CObjectLock lock(&m_cs);

		object->~ObjectType();
		char* pData = (char*)(object);
		m_FreeIndexs.push_back(pData);
	}

	//�ڴ����
	void GC(bool bEnforce = false)
	{
		CObjectLock lock(&m_cs);

		ObjectType* object = nullptr;
		char* pData = nullptr;

		// ����һ��map��ʹ����m_FreeIndexs���ӿ��һЩ
		std::unordered_map<char*, bool> findexs;
		{
			for (auto it : m_FreeIndexs)
			{
				findexs.insert(std::make_pair(it, true));
			}
		}

		//�����ڴ�
		std::vector<int> deleteList;
		deleteList.clear();

		bool bCanGC = false;
		int growSize = 0;
		
		auto it = m_FreePointerIndexs.begin(), itEnd = m_FreePointerIndexs.end();
		for (; it != itEnd; ++it)
		{
			// �����Ƿ���Ի���[���Լ���ָ���Ƿ�ȫ����m_FreeIndexs����һ���������ѷ�������һ�ݳ�ȥ�����ɻ���]
			bCanGC = true;
			for (int i = 0; i < it->first; ++i)
			{
				pData = it->second + i;
				if (findexs.find(pData) == findexs.end())
				{
					if (!bEnforce)
					{
						bCanGC = false;
						break;
					}
					else
					{
						// ǿ�ƻ�����Ҫ���յ�
						object = (ObjectType*)pData;
						object->~ObjectType();
					}
				}
			}
			// ���Ի���
			if (bCanGC)
			{
				// ���տ�������
				for (int i = 0; i < it->first; ++i)
				{
					pData = it->second + i;
					findexs.erase(pData);
				}
				// ����ָ��
				FreeFunc(static_cast<void *>(it->second));
				// ɾ����key
				deleteList.push_back(growSize);

				//��¼��ǰ����
				m_CurrentObject -= m_growSize;
				// �´μ���һ��
				m_growSize /= 2;
			}
		}

		// д�ؿ�������
		m_FreeIndexs.clear();
		for (auto it : findexs)
		{
			m_FreeIndexs.push_back(it.first);
		}
		// ����ɾ��
		int size = deleteList.size();
		for (int i = 0; i < size; ++i)
		{
			m_FreePointerIndexs.erase(deleteList[i]);
		}
	}

private:
	void Grow()
	{
		//��ǰ���������������,��������.ֱ�ӷ���.
		if (m_CurrentObject >= m_MaxObject)
		{
			return;
		}

		int objectSize = sizeof(ObjectType);

		char* pData = static_cast<char*>(MallocFunc(m_growSize * objectSize));
		if (pData == NULL) 
			return;
		// ����ָ��map��
		m_FreePointerIndexs.insert(std::make_pair(m_growSize, pData));
		// �������������
		for (int i = 0; i < m_growSize; ++i)
		{ 
			m_FreeIndexs.push_back(pData + i);
		}

		//��¼��ǰ����
		m_CurrentObject += m_growSize;
		// �´�����һ��
		m_growSize *= 2;
	}

	char* GetFreePointer()
	{
		if (m_FreeIndexs.empty())
			Grow();
		if (m_FreeIndexs.empty())
			return NULL;
		char* pData = m_FreeIndexs.back();
		m_FreeIndexs.pop_back();
		return pData;
	}
private:
	ObjectFree FreeFunc = ::free;
	ObjectMalloc MallocFunc = ::malloc;
	FreePointer  m_FreePointerIndexs;// ����ָ������map,keyΪgrowSize
	FreeIndex    m_FreeIndexs;       // ���������б�
	uint32       m_growSize;         // �ڴ������Ĵ�С
	uint32		m_MaxObject;			//����������
	uint32		m_CurrentObject;		//��ǰ�������
	::CRITICAL_SECTION m_cs;
};

//Type2Type����������֤T�Ĺ������
template<class T>
struct Type2Type {};

// ����ع���
template<class ObjectType>
class CObjectPool_Factory
{
private:
	CObjectPool_Factory()
	{
		::InitializeCriticalSection(&m_cs);
	}
public:
	~CObjectPool_Factory()
	{
		::DeleteCriticalSection(&m_cs);
	}

	// ��õ���
	static CObjectPool_Factory& GetSingleton()
	{
		static CObjectPool_Factory poolFactory;
		return poolFactory;
	}

	// ���ObjectPool
	//Type2Type��֤ ������ObjectType
	CObjectPool<ObjectType>* GetObjectPool(const Type2Type<ObjectType>& , const std::string& name)
	{
		CObjectLock lock(&m_cs);

		CObjectPool<ObjectType>* pool = nullptr;
		auto it = m_poolMap.find(name);
		if (it == m_poolMap.end())
		{
			pool = new CObjectPool<ObjectType>();
			m_poolMap.insert(std::make_pair(name, pool));
		}
		else
		{
			pool = (CObjectPool<ObjectType>*)it->second;
		}
		return pool;
	}

	// ȫ��gc
	void GC()
	{
		CObjectLock lock(&m_cs);

		for (auto it : m_poolMap)
		{
			it.second->GC(false);
		}
	}

private:
	typedef std::unordered_map<std::string, CObjectPool<ObjectType>* > PoolMap;
	PoolMap m_poolMap;
	::CRITICAL_SECTION m_cs;
};





// �����ָ�붨��
#define DefineObjectPoolPtr(T, pPool) CObjectPool<T>* pPool
// ����ض������ָ�롣
#define GetObjectPoolPtr(T) CObjectPool_Factory<T>::GetSingleton().GetObjectPool(Type2Type<T>(),#T)
// ֱ�Ӷ�������
#define ObjectPoolPtr(T, pPool) DefineObjectPoolPtr(T, pPool) = GetObjectPoolPtr(T)