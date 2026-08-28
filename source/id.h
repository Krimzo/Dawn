#pragma once

#include "util.h"

namespace dawn
{
struct ID
{
    using IntType = uint32_t;

    constexpr ID()
    {
    }

    ID(char const* str)
    {
        this->init(str);
    }

    ID(String const& str)
    {
        this->init(str);
    }

    ID(StringRef str)
    {
        this->init(str);
    }

    void init(StringRef str)
    {
        const auto it = STR_ID.find(str);
        if (it != STR_ID.end())
            m_id = it->second;
        else
        {
            m_id = (IntType)STR_ID.size() + 1; // IDs start from 1.
            STR_ID.emplace(str, m_id);
        }
    }

    constexpr IntType integer() const
    {
        return m_id;
    }

    String const& string() const
    {
        for (auto& [str, id] : STR_ID)
            if (id == m_id)
                return str;
        static const String EMPTY;
        return EMPTY;
    }

    constexpr Bool valid() const
    {
        return m_id != 0;
    }

    constexpr Bool operator==(const ID other) const
    {
        return m_id == other.m_id;
    }

    constexpr auto operator<=>(const ID other) const
    {
        return m_id <=> other.m_id;
    }

    friend std::ostream& operator<<(std::ostream& stream, ID id)
    {
        stream << id.string();
        return stream;
    }

  private:
    static constexpr Int STR_ID_RESERVE = 250;
    static inline StringMap<IntType> STR_ID;
    static inline Bool STR_ID_INIT = [] {
        STR_ID.reserve(STR_ID_RESERVE);
        return true;
    }();
    IntType m_id = 0;
};

constexpr uint64_t combine_ids(ID left, ID right)
{
    return (uint64_t(left.integer()) << 32) | right.integer();
}
} // namespace dawn

namespace std
{
template <> struct hash<dawn::ID>
{
    auto operator()(dawn::ID id) const noexcept
    {
        return std::hash<decltype(id.integer())>{}(id.integer());
    }
};
} // namespace std
