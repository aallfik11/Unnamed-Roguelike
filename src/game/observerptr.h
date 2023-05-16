#ifndef OBSERVER_PTR_H
#define OBSERVER_PTR_H
#include <ostream>

template <class T> class ObserverPtr
{
    T *ptr_;

public:
    ObserverPtr() { ptr_ = nullptr; }
    ObserverPtr(T *ptr) : ptr_{ptr} {}

    ObserverPtr(const ObserverPtr<T> &obs) { this->ptr_ = obs.ptr_; }

    ObserverPtr(ObserverPtr<T> &&obs)
    {
        this->ptr_ = obs.ptr_;
        obs.ptr_   = nullptr;
    }

    T &dereference() const { return *ptr_; }

    operator T *() const { return this->ptr_; }

    ObserverPtr<T> &operator=(T *ptr) { this->ptr_ = ptr; }

    ObserverPtr<T> &operator=(const ObserverPtr<T> &obs)
    {
        this->ptr_ = obs.ptr_;
    }

    ObserverPtr<T> &operator=(ObserverPtr &&obs)
    {
        this->ptr_ = obs.ptr_;
        obs.ptr_   = nullptr;
    }

    T &operator*() const { return *ptr_; }

    T *operator->() const { return *ptr_; }

    friend std::ostream &operator<<(std::ostream &os, const ObserverPtr<T> &obs)
    {
        os << obs.ptr_;
        return os;
    }

    friend bool operator==(const ObserverPtr<T> &obs_1,
                           const ObserverPtr<T> &obs_2)
    {
        return (obs_1.ptr_ == obs_2.ptr_);
    }
};

#endif /*OBSERVER_PTR_H*/
