#pragma once


template<class T>
class SmartPointer
{
public:
	SmartPointer();
	SmartPointer(T* obj);
	SmartPointer(const SmartPointer& other);
	~SmartPointer();
public:
	T* Get() const;
	T operator*() const;
	T* operator->() const;
	operator T*() const;
	SmartPointer* operator=(const SmartPointer& other);
private:
	void ChangeObjRef(T*, int* = nullptr);
	void SetNewObjRef(T*, int* = nullptr);
	void DecRefCount();
	void IncRefCount();
	void Init();
private:
	T* _obj;
	int* _refCount;
};


template<class T>
T* SmartPointer<T>::Get() const
{
	return _obj;
}


template<class T>
SmartPointer<T>::SmartPointer()
{
	Init();
}


template<class T>
SmartPointer<T>::SmartPointer(T* obj)
{
	Init();
	if (nullptr != obj)
	{
		SetNewObjRef(obj);
	}
}


template<class T>
SmartPointer<T>::SmartPointer(const SmartPointer<T>& other)
{
	Init();
	SetNewObjRef(other._obj, other._refCount);
}


template<class T>
void SmartPointer<T>::Init()
{
	_obj = nullptr;
	_refCount = nullptr;
}


template<class T>
SmartPointer<T>::~SmartPointer()
{
	DecRefCount();
}


template<class T>
SmartPointer<T>* SmartPointer<T>::operator=(const SmartPointer<T>& other)
{
	if (this != &other)
	{
		ChangeObjRef(other._obj, other._refCount);
	}

	return this;
}


template<class T>
void SmartPointer<T>::ChangeObjRef(T* newObjRef, int* newObjRefCount/* = nullptr*/)
{
	if (newObjRef != _obj)
	{
		DecRefCount();

		if (nullptr != newObjRef)
		{
			SetNewObjRef(newObjRef, newObjRefCount);
		}
		else
		{
			Init();
		}
	}
}


template<class T>
void SmartPointer<T>::SetNewObjRef(T* newObjRef, int* newObjRefCount/* = nullptr*/)
{
	_obj = newObjRef;

	if (nullptr != newObjRefCount)
	{
		_refCount = newObjRefCount;
	}
	else
	{
		_refCount = new int(0);
	}

	IncRefCount();
}


template<class T>
void SmartPointer<T>::DecRefCount()
{
	if (nullptr != _obj)
	{
		(*_refCount)--;
		if (0 == (*_refCount))
		{
			delete _obj;
			delete _refCount;
			Init();
		}
	}
}


template<class T>
void SmartPointer<T>::IncRefCount()
{
	(*_refCount)++;
}


//Operators binded to object
template<class T>
T SmartPointer<T>::operator*() const
{
	return *_obj;
}


template<class T>
T* SmartPointer<T>::operator->() const
{
	return _obj;
}


template<class T>
SmartPointer<T>::operator T*() const
{
	return _obj;
}


//Operators not binded to object
template<class T>
bool operator==(const SmartPointer<T>& left, std::nullptr_t right)
{
	if (right == left.Get())
	{
		return true;
	}

	return false;
}


template<class T>
bool operator==(std::nullptr_t left, const SmartPointer<T>& right)
{
	if (left == right.Get())
	{
		return true;
	}

	return false;
}


template<class T>
bool operator==(const SmartPointer<T>& left, const SmartPointer<T>& right)
{
	if (left.Get() == right.Get())
	{
		return true;
	}

	return false;
}


template<class T>
bool operator!=(const SmartPointer<T>& left, std::nullptr_t right)
{
	return !(left == right);
}


template<class T>
bool operator!=(std::nullptr_t left, const SmartPointer<T>& right)
{
	return !(left == right);
}


template<class T>
bool operator!=(const SmartPointer<T>& left, const SmartPointer<T>& right)
{
	return !(left == right);
}
