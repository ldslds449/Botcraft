#include "protocolCraft/Types/NBT/TagByteArray.hpp"

namespace ProtocolCraft
{
    TagByteArray::TagByteArray()
    {

    }

    TagByteArray::~TagByteArray()
    {

    }

    const std::vector<char>& TagByteArray::GetValues() const
    {
        return values;
    }

    void TagByteArray::SetValues(const std::vector<char> &v)
    {
        values = v;
    }

    const TagType TagByteArray::GetType() const
    {
        return TagType::ByteArray;
    }

    void TagByteArray::ReadImpl(ReadIterator &iterator, size_t &length)
    {
        ReadCollection<char, int>(iterator, length);
    }

    void TagByteArray::WriteImpl(WriteContainer &container) const
    {
        WriteCollection<char, int>(values, container);
    }

    const nlohmann::json TagByteArray::SerializeImpl() const
    {
        return nlohmann::json(values);
    }
}