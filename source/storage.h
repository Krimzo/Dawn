#pragma once

#include "t.h"

namespace dawn
{
template <typename T> struct LocalStorage
{
    LocalStorage(size_t reserver_size = 0)
    {
        m_data.reserve(reserver_size);
    }

    T& set(ID id, T const& t)
    {
        return m_data.emplace_back(id, t).second;
    }

    T* get(ID id)
    {
        for (auto it = m_data.rbegin(); it != m_data.rend(); ++it)
            if (it->first == id)
                return &it->second;
        return nullptr;
    }

    void clear()
    {
        m_data.clear();
    }

  private:
    Vector<Pair<ID, T>> m_data;
};

template <typename T> struct GlobalStorage
{
    GlobalStorage(size_t initial_size = 0)
    {
        m_data.resize(initial_size);
    }

    T& set(ID id, T const& t)
    {
        if (id.integer() >= m_data.size())
            m_data.resize(size_t(id.integer() + 1) * 2);
        return m_data[id.integer()].emplace(t);
    }

    T* get(ID id)
    {
        if (id.integer() < m_data.size())
        {
            if (auto& opt_obj = m_data[id.integer()])
                return &(*opt_obj);
        }
        return nullptr;
    }

    void clear()
    {
        m_data.clear();
    }

  private:
    Vector<Opt<T>> m_data;
};
} // namespace dawn
