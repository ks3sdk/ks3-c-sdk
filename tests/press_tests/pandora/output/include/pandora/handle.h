/*
 * @landyliu
*/

#ifndef PANDORA_HANDLE_H
#define PANDORA_HANDLE_H

#include <algorithm>
#include <assert.h>

namespace pandora
{
/*
 * HandleBase is a template class.
 * It has a param T, which is the object to be managed.
 * It is the base class of Handle, so do not use this class directly
 */
template<typename T>
class HandleBase
{
public:

    typedef T element_type;
    
    T* Get() const
    {
        return ptr_;
    }

    T* operator->() const
    {
        if(!ptr_)
        {
            ThrowNullHandleException(__FILE__, __LINE__);          
        }
        return ptr_;
    }

    T& operator*() const
    {
        if(!ptr_)
        {
            ThrowNullHandleException(__FILE__, __LINE__);           
        }

        return *ptr_;
    }

    operator bool() const
    {
        return ptr_ ? true : false;
    }

    void Swap(HandleBase& other)
    {
        std::swap(ptr_, other.ptr_);
    }

    T* ptr_;

private:

    void ThrowNullHandleException(const char *, int) const;
};

template<typename T> inline void 
HandleBase<T>::ThrowNullHandleException(const char* file, int line) const
{
    assert(!"NullHandleException");
}

template<typename T, typename U>
inline bool operator==(const HandleBase<T>& lhs, const HandleBase<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
    if(l && r)
    {
        return *l == *r;
    }
    return !l && !r;
}

template<typename T, typename U>
inline bool operator!=(const HandleBase<T>& lhs, const HandleBase<U>& rhs)
{
    return !operator==(lhs, rhs);
}

template<typename T, typename U>
inline bool operator<(const HandleBase<T>& lhs, const HandleBase<U>& rhs)
{
    T* l = lhs.Get();
    U* r = rhs.Get();
    if(l && r)
    {
        return *l < *r;
    }

    return !l && r;
}

template<typename T, typename U>
inline bool operator<=(const HandleBase<T>& lhs, const HandleBase<U>& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<typename T, typename U>
inline bool operator>(const HandleBase<T>& lhs, const HandleBase<U>& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, typename U>
inline bool operator>=(const HandleBase<T>& lhs, const HandleBase<U>& rhs)
{
    return !(lhs < rhs);
}

/*
 * Handle is a template class.
 * It has a param T, which is the object to be managed.
 * Handle is mainly used for impl smart pointer.
 */
template<typename T>
class Handle : public HandleBase<T>
{
public:
    
    Handle(T* p = 0)
    {
        this->ptr_ = p;

        if(this->ptr_)
        {
            this->ptr_->IncRef();
        }
    }
    
    template<typename Y>
    Handle(const Handle<Y>& r)
    {
        this->ptr_ = r.ptr_;

        if(this->ptr_)
        {
            this->ptr_->IncRef();
        }
    }

    Handle(const Handle& r)
    {
        this->ptr_ = r.ptr_;

        if(this->ptr_)
        {
            this->ptr_->IncRef();
        }
    }
    
    ~Handle()
    {
        if(this->ptr_)
        {
            this->ptr_->DecRef();
        }
    }
    
    Handle& operator=(T* p)
    {
        if(this->ptr_ != p)
        {
            if(p)
            {
                p->IncRef();
            }

            T* ptr = this->ptr_;
            this->ptr_ = p;

            if(ptr)
            {
                ptr->DecRef();
            }
        }
        return *this;
    }
        
    template<typename Y>
    Handle& operator=(const Handle<Y>& r)
    {
        if(this->ptr_ != r.ptr_)
        {
            if(r.ptr_)
            {
                r.ptr_->IncRef();
            }

            T* ptr = this->ptr_;
            this->ptr_ = r.ptr_;

            if(ptr)
            {
                ptr->DecRef();
            }
        }
        return *this;
    }

    Handle& operator=(const Handle& r)
    {
        if(this->ptr_ != r.ptr_)
        {
            if(r.ptr_)
            {
                r.ptr_->IncRef();
            }

            T* ptr = this->ptr_;
            this->ptr_ = r.ptr_;

            if(ptr)
            {
                ptr->DecRef();
            }
        }
        return *this;
    }
        
    template<class Y>
    static Handle DynamicCast(const HandleBase<Y>& r)
    {
        return Handle(dynamic_cast<T*>(r.ptr_));
    }

    template<class Y>
    static Handle DynamicCast(Y* p)
    {
        return Handle(dynamic_cast<T*>(p));
    }
};
}//end namespace
#endif
