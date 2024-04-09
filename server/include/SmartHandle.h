#pragma once

#include <windows.h>

// Smart pointer wrapper (similar in behaviour to std::unique_ptr)
// used for windows HANDLE management
class SmartHandle
{
private:
    HANDLE m_handle{nullptr};
public:
    explicit SmartHandle(HANDLE handle);
    ~SmartHandle();

    //move constructor
    SmartHandle(SmartHandle &&other) noexcept;

    //move operator (transfer ownership of HANDLE)
    SmartHandle& operator=(SmartHandle &&other) noexcept;

    //conversion operator
    //used to transfer ownership even if a function expects a raw HANDLE instead of a SmartHandle
    // operator HANDLE() && {
    //     return std::exchange(m_handle, nullptr);    //return the handle and set it to null afterwards. Avoids creating a temporary var
    // }

    //Disable copying
    SmartHandle(const SmartHandle& other) = delete;
    SmartHandle& operator=(const SmartHandle &other) = delete;

    void reset(HANDLE handle);
    HANDLE get() const;
    void close();
};