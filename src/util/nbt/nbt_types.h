#ifndef NBT_TYPES_H
#define NBT_TYPES_H

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <type_traits>

namespace nbt {

    class NBT;
    class NBTList;
	class NBTCompound;

    using nbt_value = NBT;
    using nbt_byte = int8_t; // nbt bytes are signed
    using nbt_short = int16_t;
    using nbt_int = int32_t;
    using nbt_long = int64_t;
    using nbt_float = float;
    using nbt_double = double;
    using nbt_byte_array = std::vector<nbt_byte>;
    using nbt_string = std::string;
    using nbt_string_view = std::string_view;
    using nbt_list = NBTList;
    using nbt_compound = NBTCompound;
    using nbt_int_array = std::vector<nbt_int>;
    using nbt_long_array = std::vector<nbt_long>;

    static_assert(sizeof(nbt_byte) == 1);
    static_assert(sizeof(nbt_short) == 2);
    static_assert(sizeof(nbt_int) == 4);
    static_assert(sizeof(nbt_long) == 8);
    static_assert(sizeof(nbt_float) == 4);
    static_assert(sizeof(nbt_double) == 8);
    static_assert(std::numeric_limits<nbt_float>::is_iec559);
    static_assert(std::numeric_limits<nbt_double>::is_iec559);

    enum class TagType {
        End = 0,
        Byte = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        ByteArray = 7,
        String = 8,
        List = 9,
        Compound = 10,
        IntArray = 11,
        LongArray = 12,
    };
    inline constexpr TagType TagEnd = TagType::End;
    inline constexpr TagType TagByte = TagType::Byte;
    inline constexpr TagType TagShort = TagType::Short;
    inline constexpr TagType TagInt = TagType::Int;
    inline constexpr TagType TagLong = TagType::Long;
    inline constexpr TagType TagFloat = TagType::Float;
    inline constexpr TagType TagDouble = TagType::Double;
    inline constexpr TagType TagByteArray = TagType::ByteArray;
    inline constexpr TagType TagString = TagType::String;
    inline constexpr TagType TagList = TagType::List;
    inline constexpr TagType TagCompound = TagType::Compound;
    inline constexpr TagType TagIntArray = TagType::IntArray;
    inline constexpr TagType TagLongArray = TagType::LongArray;

    std::string to_string(const TagType& tagType);
    std::ostream& operator <<(std::ostream&, const TagType&);

    inline constexpr int getTagTypeId(TagType tagType) noexcept {
        return static_cast<int>(tagType);
    }


    template<TagType tagType> struct get_nbt_type;
    template<> struct get_nbt_type<TagByte> { using type = nbt_byte; };
    template<> struct get_nbt_type<TagShort> { using type = nbt_short; };
    template<> struct get_nbt_type<TagInt> { using type = nbt_int; };
    template<> struct get_nbt_type<TagLong> { using type = nbt_long; };
    template<> struct get_nbt_type<TagFloat> { using type = nbt_float; };
    template<> struct get_nbt_type<TagDouble> { using type = nbt_double; };
    template<> struct get_nbt_type<TagByteArray> { using type = nbt_byte_array; };
    template<> struct get_nbt_type<TagString> { using type = nbt_string; };
    template<> struct get_nbt_type<TagList> { using type = nbt_list; };
    template<> struct get_nbt_type<TagCompound> { using type = nbt_compound; };
    template<> struct get_nbt_type<TagIntArray> { using type = nbt_int_array; };
    template<> struct get_nbt_type<TagLongArray> { using type = nbt_long_array; };
    template<TagType tagType> using nbt_type = typename get_nbt_type<tagType>::type;

    template<typename T> struct get_nbt_tag_type;
    template<> struct get_nbt_tag_type<nbt_byte> { static constexpr TagType tag_type = TagByte; };
    template<> struct get_nbt_tag_type<nbt_short> { static constexpr TagType tag_type = TagShort; };
    template<> struct get_nbt_tag_type<nbt_int> { static constexpr TagType tag_type = TagInt; };
    template<> struct get_nbt_tag_type<nbt_long> { static constexpr TagType tag_type = TagLong; };
    template<> struct get_nbt_tag_type<nbt_float> { static constexpr TagType tag_type = TagFloat; };
    template<> struct get_nbt_tag_type<nbt_double> { static constexpr TagType tag_type = TagDouble; };
    template<> struct get_nbt_tag_type<nbt_byte_array> { static constexpr TagType tag_type = TagByteArray; };
    template<> struct get_nbt_tag_type<nbt_string> { static constexpr TagType tag_type = TagString; };
    template<> struct get_nbt_tag_type<nbt_list> { static constexpr TagType tag_type = TagList; };
    template<> struct get_nbt_tag_type<nbt_compound> { static constexpr TagType tag_type = TagCompound; };
    template<> struct get_nbt_tag_type<nbt_int_array> { static constexpr TagType tag_type = TagIntArray; };
    template<> struct get_nbt_tag_type<nbt_long_array> { static constexpr TagType tag_type = TagLongArray; };
    template<typename T> inline constexpr TagType nbt_tag_type = get_nbt_tag_type<T>::tag_type;

    template<typename T> struct is_nbt_type : std::false_type {};
	template<> struct is_nbt_type<nbt_type<TagByte>> : std::true_type {};
	template<> struct is_nbt_type<nbt_type<TagShort>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagInt>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagLong>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagFloat>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagDouble>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagByteArray>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagString>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagList>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagCompound>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagIntArray>> : std::true_type {};
    template<> struct is_nbt_type<nbt_type<TagLongArray>> : std::true_type {};
	template<typename T> inline constexpr bool is_nbt_type_v = is_nbt_type<T>::value;


    // returns -1 if the size is encoded in the tag data
    constexpr size_t getPayloadSize(TagType tagType) noexcept {
        switch (tagType) {
            case TagEnd: return 0;
            case TagByte: return sizeof(nbt_type<TagByte>);
            case TagShort: return sizeof(nbt_type<TagShort>);
            case TagInt: return sizeof(nbt_type<TagInt>);
            case TagLong: return sizeof(nbt_type<TagLong>);
            case TagFloat: return sizeof(nbt_type<TagFloat>);
            case TagDouble: return sizeof(nbt_type<TagDouble>);
            case TagByteArray:
            case TagString:
            case TagList:
            case TagCompound:
            case TagIntArray:
            case TagLongArray:
                return size_t(-1);
        }
        return 0;
    }

}

#endif