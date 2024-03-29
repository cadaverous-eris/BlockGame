#include "nbt_types.h"

namespace nbt {

    std::string to_string(const TagType& tagType) {
        switch (tagType) {
            case TagEnd: return "TAG_End";
            case TagByte: return "TAG_Byte";
            case TagShort: return "TAG_Short";
            case TagInt: return "TAG_Int";
            case TagLong: return "TAG_Long";
            case TagFloat: return "TAG_Float";
            case TagDouble: return "TAG_Double";
            case TagString: return "TAG_String";
            case TagList: return "TAG_List";
            case TagCompound: return "TAG_Compound";
        }
        if (tagType == TagType{ 7 })
            return "TAG_Byte_Array";
        else if (tagType == TagType{ 11 })
            return "TAG_Int_Array";
        else if (tagType == TagType{ 12 })
            return "TAG_Long_Array";
        return "Invalid Tag Type";
    }

    std::ostream& operator <<(std::ostream& os, const TagType& tagType) {
        switch (tagType) {
            case TagEnd: return os << "TAG_End";
            case TagByte: return os << "TAG_Byte";
            case TagShort: return os << "TAG_Short";
            case TagInt: return os << "TAG_Int";
            case TagLong: return os << "TAG_Long";
            case TagFloat: return os << "TAG_Float";
            case TagDouble: return os << "TAG_Double";
            case TagString: return os << "TAG_String";
            case TagList: return os << "TAG_List";
            case TagCompound: return os << "TAG_Compound";
        }
        if (tagType == TagType{ 7 })
            return os << "TAG_Byte_Array";
        else if (tagType == TagType{ 11 })
            return os << "TAG_Int_Array";
        else if (tagType == TagType{ 12 })
            return os << "TAG_Long_Array";
        return os << "Invalid Tag Type";
    }

}