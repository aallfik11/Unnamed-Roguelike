#ifndef OBSERVER_PTR_H
#define OBSERVER_PTR_H
#include <ostream>

template <class T> class observer_ptr
{
    T *ptr_;

public:
    observer_ptr() { ptr_ = nullptr; }
    observer_ptr(T *const ptr) : ptr_{ptr} {}
    observer_ptr(std::nullptr_t n) : ptr_{n} {}

    observer_ptr(const observer_ptr<T> &obs) { this->ptr_ = obs.ptr_; }

    observer_ptr(observer_ptr<T> &&obs)
    {
        this->ptr_ = obs.ptr_;
        obs.ptr_   = nullptr;
    }

    T &dereference() const { return *ptr_; }
    T *get() const { return ptr_; }

    operator T *() const { return this->ptr_; }

    observer_ptr<T> &operator=(T *const ptr) { this->ptr_ = ptr; }

    observer_ptr<T> &operator=(const observer_ptr<T> &obs)
    {
        this->ptr_ = obs.ptr_;
    }

    observer_ptr<T> &operator=(observer_ptr<T> &&obs)
    {
        this->ptr_ = obs.ptr_;
        obs.ptr_   = nullptr;
    }

    T &operator*() const { return *ptr_; }

    T *operator->() const { return *ptr_; }

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
        return (obs.ptr == n);
    }

    friend bool operator==(std::nullptr_t         n,
                           const observer_ptr<T> &obs) noexcept
    {
        return (n == obs.ptr_);
    }
};

namespace std
{
template <class T> struct hash<observer_ptr<T>>
{
    size_t operator()(observer_ptr<T> obs) const noexcept
    {
        return hash<T *>()(obs.get());
    }
};
} // namespace std

#endif /*OBSERVER_PTR_H*/
