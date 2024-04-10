#include "SmartHandle.h"

SmartHandle::SmartHandle(HANDLE handle) : m_handle(handle) {}

SmartHandle::~SmartHandle() { close(); }

//move constructor
SmartHandle::SmartHandle(SmartHandle &&other) noexcept {
    m_handle = other.m_handle;
    other.m_handle = nullptr;
}

//move operator
SmartHandle &SmartHandle::operator=(SmartHandle &&other) noexcept {
    if (this != &other) {
        reset(other.m_handle);
        other.m_handle = nullptr;
    }

    return *this;
}

void SmartHandle::reset(HANDLE handle) {
    close();
    m_handle = handle;
}

HANDLE SmartHandle::get() const {
    return m_handle;
}

void SmartHandle::close() {
    if (m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
        m_handle = nullptr;
    }
}