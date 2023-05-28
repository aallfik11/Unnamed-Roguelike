#ifndef OBSERVER_PTR_H
#define OBSERVER_PTR_H
#include <ostream>

template <class T> class observer_ptr
{
    T *ptr_;

public:
    observer_ptr() { ptr_ = nullptr; }
    observer_ptr(T *const ptr) : ptr_{ptr} {}
    observer_ptr(const T &ref) { ptr_ = &ref; }
    observer_ptr(std::nullptr_t n) : ptr_{n} {}

    observer_ptr(const observer_ptr<T> &obs) { this->ptr_ = obs.ptr_; }

    observer_ptr(observer_ptr<T> &&obs)
    {
        if (*this != obs)
        {
            this->ptr_ = obs.ptr_;
            obs.ptr_   = nullptr;
        }
    }
    // T *get() const { return this->ptr_; }

    T &dereference() const { return *ptr_; }

    observer_ptr<const T> getConst() const
    {
        return static_cast<const T *>(this->ptr_);
    }

    operator T *() const { return this->ptr_; }

    observer_ptr<T> &operator=(T *const ptr)
    {
        this->ptr_ = ptr;
        return *this;
    }

    observer_ptr<T> &operator=(const observer_ptr<T> &obs)
    {
        this->ptr_ = obs.ptr_;
        return *this;
    }

    observer_ptr<T> &operator=(observer_ptr<T> &&obs)
    {
        if (*this == obs)
        {
            return *this;
        }
        this->ptr_ = obs.ptr_;
        obs.ptr_   = nullptr;
        return *this;
    }

    T &operator*() const { return *ptr_; }

    T *operator->() const { return ptr_; }

    T& operator[] (int index) const {return ptr_[index];}

    friend std::ostream &operator<<(std::ostream          &os,
                                    const observer_ptr<T> &obs)
    {
        os << obs.ptr_;
        return os;
    }

    friend bool operator==(const observer_ptr<T> &obs_1,
                           const observer_ptr<T> &obs_2)
    {
        return (obs_1.ptr_ == obs_2.ptr_);
    }

    friend bool operator==(const observer_ptr<T> &obs,
                           std::nullptr_t         n) noexcept
    {
        return (obs.ptr_ == n);
    }

    friend bool operator==(std::nullptr_t         n,
                           const observer_ptr<T> &obs) noexcept
    {
        return (n == obs.ptr_);
    }
    template <class C, class U>
    friend observer_ptr<C> static_observer_cast(const observer_ptr<U> &obs);

    template <class C, class U>
    friend observer_ptr<C> dynamic_observer_cast(const observer_ptr<U> &obs);

    template <class C, class U>
    friend observer_ptr<C> const_observer_cast(const observer_ptr<U> &obs);
    template <class C, class U>
    friend observer_ptr<C>
    reinterpret_observer_cast(const observer_ptr<U> &obs);

    
    friend std::hash<observer_ptr<T>>;
};

template <class T, class U>
observer_ptr<T> static_observer_cast(const observer_ptr<U> &obs)
{
    auto p = static_cast<T *>(obs.ptr_);
    return observer_ptr<T>(p);
}
template <class T, class U>
observer_ptr<T> dynamic_observer_cast(const observer_ptr<U> &obs)
{

    auto p = dynamic_cast<T *>(obs.ptr_);
    return observer_ptr<T>(p);
}
template <class T, class U>
observer_ptr<T> const_observer_cast(const observer_ptr<U> &obs)
{
    auto p = const_cast<T *>(obs.ptr_);
    return observer_ptr<T>(p);
}
template <class T, class U>
observer_ptr<T> reinterpret_observer_cast(const observer_ptr<U> &obs)
{
    auto p = reinterpret_cast<T *>(obs.ptr_);
    return observer_ptr<T>(p);
}

namespace std
{
template <class T> struct hash<observer_ptr<T>>
{
    size_t operator()(observer_ptr<T> obs) const noexcept
    {
        return hash<T *>()(obs.ptr_);
    }
};
} // namespace std

#endif /*OBSERVER_PTR_H*/
