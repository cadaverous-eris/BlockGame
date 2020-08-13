#include "nbt_types.h"

namespace nbt {

    std::ostream& operator <<(std::ostream& os, const TagType& tagType) {
        switch (tagType) {
            case TagEnd: return os << "TAG_End";
            case TagByte: return os << "TAG_Byte";
            case TagShort: return os << "TAG_Short";
            case TagInt: return os << "TAG_Int";
            case TagLong: return os << "TAG_Long";
            case TagFloat: return os << "TAG_Float";
            case TagDouble: return os << "TAG_Double";
            case TagByteArray: return os << "TAG_Byte_Array";
            case TagString: return os << "TAG_String";
            case TagList: return os << "TAG_List";
            case TagCompound: return os << "TAG_Compound";
            case TagIntArray: return os << "TAG_Int_Array";
            case TagLongArray: return os << "TAG_Long_Array";
        }
        return os << "Invalid Tag Type";
    }

}