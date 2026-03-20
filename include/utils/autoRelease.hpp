#pragma once

#include <functional>
#include <ranges>

template <class T, T Invalid = {}>
class AutoRelease
{
public:
    AutoRelease()
        : AutoRelease(Invalid, nullptr)
    {
    }

    AutoRelease(T obj, std::function<void(T)> releaseFunc)
        : obj_(obj), releaseFunc_(releaseFunc)
    {
    }

    ~AutoRelease()
    {
        if ((obj_ != Invalid) && releaseFunc_)
        {
            releaseFunc_(obj_);
        }
    }

    // Disable copy semantics
    AutoRelease(const AutoRelease &) = delete;
    AutoRelease &operator=(const AutoRelease &) = delete;

    // Enable move semantics
    AutoRelease(AutoRelease &&other)
        : AutoRelease()
    {
        swap(other);
    }
    AutoRelease &operator=(AutoRelease &&other)
    {
        AutoRelease new_obj(std::move(other));
        swap(new_obj);
        return *this;
    }

    void swap(AutoRelease &other) noexcept
    {
        std::ranges::swap(obj_, other.obj_);
        std::ranges::swap(releaseFunc_, other.releaseFunc_);
    }

    T get() const { return obj_; }

    operator T() const { return obj_; }

    T *operator&() noexcept
    {
        return std::addressof(obj_);
    }

private:
    T obj_;
    std::function<void(T)> releaseFunc_;
};