#pragma once 

#include <type_traits>
#include <cassert>

#define	NIV_COM_PTR_AddRef(p) p->AddRef ()
#define	NIV_COM_PTR_Release(p) p->Release ()

template<typename T> 
struct ComPtr 
{ 
public: 
 
	explicit ComPtr(T* lComPtr = nullptr) : ptr_(lComPtr) 
	{ 
		static_assert(std::is_base_of<IUnknown, T>::value, "T needs to be IUnknown based"); 
		if (ptr_) 
		{ 
			NIV_COM_PTR_AddRef (ptr_); 
		} 
	} 
 
	ComPtr(const ComPtr<T>& lComPtrObj) 
	{ 
		static_assert(std::is_base_of<IUnknown, T>::value, "T needs to be IUnknown based"); 
		ptr_ = lComPtrObj.ptr_; 
 
		if (ptr_) 
		{ 
			NIV_COM_PTR_AddRef(ptr_); 
		} 
	}
 
	template <typename U>
	ComPtr (const ComPtr<U>& lComPtrObj) 
	{ 
		static_assert(std::is_base_of<T, U>::value, "U needs to be T based"); 
		ptr_ = lComPtrObj.GetInterface (); 
 
		if (ptr_) 
		{ 
			NIV_COM_PTR_AddRef (ptr_);
		} 
	} 
 
	ComPtr(ComPtr<T>&& lComPtrObj) 
	{ 
		ptr_ = lComPtrObj.ptr_; 
		lComPtrObj.ptr_ = nullptr; 
	} 
 
	T* operator=(T* lComPtr) 
	{ 
		if (ptr_) 
		{ 
			NIV_COM_PTR_Release (ptr_);
		} 
 
		ptr_ = lComPtr; 
 
		if (ptr_) 
		{ 
			NIV_COM_PTR_AddRef (ptr_);
		} 
 
		return ptr_; 
	} 
 
	T* operator=(const ComPtr<T>& lComPtrObj) 
	{ 
		if (ptr_) 
		{ 
			NIV_COM_PTR_Release (ptr_);
		} 
 
		ptr_ = lComPtrObj.ptr_; 
 
		if (ptr_) 
		{ 
			NIV_COM_PTR_AddRef (ptr_);
		} 
 
		return ptr_; 
	} 
 
	~ComPtr() 
	{ 
		if (ptr_) 
		{ 
			NIV_COM_PTR_Release (ptr_);
			ptr_ = nullptr; 
		} 
	} 
 
	operator T*() const 
	{ 
		return ptr_; 
	} 
 
	T* GetInterface() const 
	{ 
		return ptr_; 
	} 
 
	T& operator*() const 
	{ 
		return *ptr_; 
	} 
 
	T** operator&() 
	{ 
		//The assert on operator& usually indicates a bug. Could be a potential memory leak. 
		// If this really what is needed, however, use GetInterface() explicitly. 
		assert(nullptr == ptr_); 
		return &ptr_; 
	} 
 
	T* operator->() const 
	{ 
		return ptr_; 
	} 
 
	bool operator!() const 
	{     
		return (nullptr == ptr_); 
	} 
 
	bool operator<(T* lComPtr) const 
	{ 
		return ptr_ < lComPtr; 
	} 
 
	bool operator!=(T* lComPtr) const 
	{ 
		return !operator==(lComPtr); 
	} 
 
	bool operator==(T* lComPtr) const 
	{ 
		return ptr_ == lComPtr; 
	} 
 
	template <typename I> 
	HRESULT QueryInterface(I **interfacePtr) 
	{ 
		return ptr_->QueryInterface(IID_PPV_ARGS(interfacePtr)); 
	}

	T* Retrieve ()
	{
		NIV_COM_PTR_AddRef (ptr_);
		return ptr_;
	}
 
protected: 
	// The internal interface pointer 
	T* ptr_; 
}; 

#undef NIV_COM_PTR_AddRef
#undef NIV_COM_PTR_Release