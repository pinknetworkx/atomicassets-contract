#pragma once

#include <eosio/eosio.hpp>
#include "base58.hpp"

using namespace eosio;
using namespace std;

namespace atomicdata {

    //Custom vector types need to be defined because otherwise a bug in the ABI serialization
    //would cause the ABI to be invalid
    typedef std::vector <int8_t> INT8_VEC;
    typedef std::vector <int16_t> INT16_VEC;
    typedef std::vector <int32_t> INT32_VEC;
    typedef std::vector <int64_t> INT64_VEC;
    typedef std::vector <uint8_t> UINT8_VEC;
    typedef std::vector <uint16_t> UINT16_VEC;
    typedef std::vector <uint32_t> UINT32_VEC;
    typedef std::vector <uint64_t> UINT64_VEC;
    typedef std::vector <float> FLOAT_VEC;
    typedef std::vector <double> DOUBLE_VEC;
    typedef std::vector <std::string> STRING_VEC;

    typedef std::variant <\
        int8_t, int16_t, int32_t, int64_t, \
        uint8_t, uint16_t, uint32_t, uint64_t, \
        float, double, std::string, \
        atomicdata::INT8_VEC, atomicdata::INT16_VEC, atomicdata::INT32_VEC, atomicdata::INT64_VEC, \
        atomicdata::UINT8_VEC, atomicdata::UINT16_VEC, atomicdata::UINT32_VEC, atomicdata::UINT64_VEC, \
        atomicdata::FLOAT_VEC, atomicdata::DOUBLE_VEC, atomicdata::STRING_VEC> ATOMIC_ATTRIBUTE;

    typedef std::map <std::string, ATOMIC_ATTRIBUTE> ATTRIBUTE_MAP;

    struct FORMAT {
        std::string name;
        std::string type;
    };

    static constexpr uint64_t RESERVED = 4;


    vector <uint8_t> toVarintBytes(uint64_t number, uint64_t original_bytes = 8) {
        if (original_bytes < 8) {
            uint64_t bitmask = ((uint64_t) 1 << original_bytes * 8) - 1;
            number &= bitmask;
        }

        vector <uint8_t> bytes = {};
        while (number >= 128) {
            // sets msb, stores remainder in lower bits
            bytes.push_back((uint8_t)(128 + number % 128));
            number /= 128;
        }
        bytes.push_back((uint8_t) number);

        return bytes;
    }

    uint64_t unsignedFromVarintBytes(vector <const uint8_t>::iterator &itr) {
        uint64_t number = 0;
        uint64_t multiplier = 1;

        while (*itr >= 128) {
            number += (((uint64_t) * itr) - 128) * multiplier;
            itr++;
            multiplier *= 128;
        }
        number += ((uint64_t) * itr) * multiplier;
        itr++;

        return number;
    }

    //It is expected that the number is smaller than 2^byte_amount
    vector <uint8_t> toIntBytes(uint64_t number, uint64_t byte_amount) {
        vector <uint8_t> bytes = {};
        for (uint64_t i = 0; i < byte_amount; i++) {
            bytes.push_back((uint8_t) number % 256);
            number /= 256;
        }
        return bytes;
    }

    uint64_t unsignedFromIntBytes(vector <const uint8_t>::iterator &itr, uint64_t original_bytes = 8) {
        uint64_t number = 0;
        uint64_t multiplier = 1;

        for (uint64_t i = 0; i < original_bytes; i++) {
            number += ((uint64_t) * itr) * multiplier;
            multiplier *= 256;
            itr++;
        }

        return number;
    }


    uint64_t zigzagEncode(int64_t value) {
        if (value < 0) {
            return (uint64_t)(-1 * (value + 1)) * 2 + 1;
        } else {
            return (uint64_t) value * 2;
        }
    }

    int64_t zigzagDecode(uint64_t value) {
        if (value % 2 == 0) {
            return (int64_t)(value / 2);
        } else {
            return (int64_t)(value / 2) * -1 - 1;
        }
    }


    vector <uint8_t> serialize_attribute(const string &type, const ATOMIC_ATTRIBUTE &attr) {
        if (type.find("[]", type.length() - 2) == type.length() - 2) {
            //Type is an array
            string base_type = type.substr(0, type.length() - 2);

            if (std::holds_alternative <INT8_VEC>(attr)) {
                INT8_VEC vec = std::get <INT8_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <INT16_VEC>(attr)) {
                INT16_VEC vec = std::get <INT16_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <INT32_VEC>(attr)) {
                INT32_VEC vec = std::get <INT32_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <INT64_VEC>(attr)) {
                INT64_VEC vec = std::get <INT64_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <UINT8_VEC>(attr)) {
                UINT8_VEC vec = std::get <UINT8_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <UINT16_VEC>(attr)) {
                UINT16_VEC vec = std::get <UINT16_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <UINT32_VEC>(attr)) {
                UINT32_VEC vec = std::get <UINT32_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <UINT64_VEC>(attr)) {
                UINT64_VEC vec = std::get <UINT64_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <FLOAT_VEC>(attr)) {
                FLOAT_VEC vec = std::get <FLOAT_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <DOUBLE_VEC>(attr)) {
                DOUBLE_VEC vec = std::get <DOUBLE_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            } else if (std::holds_alternative <STRING_VEC>(attr)) {
                STRING_VEC vec = std::get <STRING_VEC>(attr);
                vector <uint8_t> serialized_data = toVarintBytes(vec.size());
                for (auto child : vec) {
                    ATOMIC_ATTRIBUTE child_attr = child;
                    vector <uint8_t> serialized_element = serialize_attribute(base_type, child_attr);
                    serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
                }
                return serialized_data;

            }
        }

        if (type == "int8") {
            check(std::holds_alternative <int8_t>(attr), "Expected a int8, but got something else");
            return toVarintBytes(zigzagEncode(std::get <int8_t>(attr)), 1);
        } else if (type == "int16") {
            check(std::holds_alternative <int16_t>(attr), "Expected a int16, but got something else");
            return toVarintBytes(zigzagEncode(std::get <int16_t>(attr)), 2);
        } else if (type == "int32") {
            check(std::holds_alternative <int32_t>(attr), "Expected a int32, but got something else");
            return toVarintBytes(zigzagEncode(std::get <int32_t>(attr)), 4);
        } else if (type == "int64") {
            check(std::holds_alternative <int64_t>(attr), "Expected a int64, but got something else");
            return toVarintBytes(zigzagEncode(std::get <int64_t>(attr)), 8);

        } else if (type == "uint8") {
            check(std::holds_alternative <uint8_t>(attr), "Expected a uint8, but got something else");
            return toVarintBytes(std::get <uint8_t>(attr), 1);
        } else if (type == "uint16") {
            check(std::holds_alternative <uint16_t>(attr), "Expected a uint16, but got something else");
            return toVarintBytes(std::get <uint16_t>(attr), 2);
        } else if (type == "uint32") {
            check(std::holds_alternative <uint32_t>(attr), "Expected a uint32, but got something else");
            return toVarintBytes(std::get <uint32_t>(attr), 4);
        } else if (type == "uint64") {
            check(std::holds_alternative <uint64_t>(attr), "Expected a uint64, but got something else");
            return toVarintBytes(std::get <uint64_t>(attr), 8);

        } else if (type == "fixed8" || type == "byte") {
            check(std::holds_alternative <uint8_t>(attr), "Expected a uint8 (fixed8 / byte), but got something else");
            return toIntBytes(std::get <uint8_t>(attr), 1);
        } else if (type == "fixed16") {
            check(std::holds_alternative <uint16_t>(attr), "Expected a uint16 (fixed16), but got something else");
            return toIntBytes(std::get <uint16_t>(attr), 2);
        } else if (type == "fixed32") {
            check(std::holds_alternative <uint32_t>(attr), "Expected a uint32 (fixed32), but got something else");
            return toIntBytes(std::get <uint32_t>(attr), 4);
        } else if (type == "fixed64") {
            check(std::holds_alternative <uint64_t>(attr), "Expected a uint64 (fixed64), but got something else");
            return toIntBytes(std::get <uint64_t>(attr), 8);

        } else if (type == "float") {
            check(std::holds_alternative <float>(attr), "Expected a float, but got something else");
            float float_value = std::get <float>(attr);
            auto *byte_value = reinterpret_cast<uint8_t *>(&float_value);
            vector <uint8_t> serialized_data = {};
            serialized_data.reserve(4);
            for (int i = 0; i < 4; i++) {
                serialized_data.push_back(*(byte_value + i));
            }
            return serialized_data;

        } else if (type == "double") {
            check(std::holds_alternative <double>(attr), "Expected a double, but got something else");
            double float_value = std::get <double>(attr);
            auto *byte_value = reinterpret_cast<uint8_t *>(&float_value);
            vector <uint8_t> serialized_data = {};
            serialized_data.reserve(8);
            for (int i = 0; i < 8; i++) {
                serialized_data.push_back(*(byte_value + i));
            }
            return serialized_data;

        } else if (type == "string" || type == "image") {
            check(std::holds_alternative <string>(attr), "Expected a string, but got something else");
            string text = std::get <string>(attr);
            vector <uint8_t> serialized_data(text.begin(), text.end());

            vector <uint8_t> length_bytes = toVarintBytes(text.length());
            serialized_data.insert(serialized_data.begin(), length_bytes.begin(), length_bytes.end());
            return serialized_data;

        } else if (type == "ipfs") {
            check(std::holds_alternative <string>(attr), "Expected a string (ipfs), but got something else");
            vector <uint8_t> result = {};
            check(DecodeBase58(std::get <string>(attr), result),
                "Error when decoding IPFS string");
            vector <uint8_t> length_bytes = toVarintBytes(result.size());
            result.insert(result.begin(), length_bytes.begin(), length_bytes.end());
            return result;

        } else if (type == "bool") {
            check(std::holds_alternative <uint8_t>(attr),
                "Expected a bool (needs to be provided as uint8_t because of C++ restrictions), but got something else");
            uint8_t value = std::get <uint8_t>(attr);
            check(value == 0 || value == 1,
                "Bools need to be provided as an uin8_t that is either 0 or 1");
            return {value};

        } else {
            check(false, "No type could be matched - " + type);
            return {}; //This point can never be reached because the check above will always throw.
            //Just to silence the compiler warning
        }
    }


    ATOMIC_ATTRIBUTE deserialize_attribute(const string &type, vector <const uint8_t>::iterator &itr) {
        if (type.find("[]", type.length() - 2) == type.length() - 2) {
            //Type is an array
            uint64_t array_length = unsignedFromVarintBytes(itr);
            string base_type = type.substr(0, type.length() - 2);

            if (type == "int8[]") {
                INT8_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int8_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "int16[]") {
                INT16_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int16_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "int32[]") {
                INT32_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int32_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "int64[]") {
                INT64_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <int64_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "uint8[]" || type == "fixed8[]" || type == "bool[]") {
                UINT8_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint8_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "uint16[]" || type == "fixed16[]") {
                UINT16_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint16_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "uint32[]" || type == "fixed32[]") {
                UINT32_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint32_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;
            } else if (type == "uint64[]" || type == "fixed64[]") {
                UINT64_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <uint64_t>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "float[]") {
                FLOAT_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <float>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "double[]") {
                DOUBLE_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <double>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            } else if (type == "string[]" || type == "image[]") {
                STRING_VEC vec = {};
                for (uint64_t i = 0; i < array_length; i++) {
                    vec.push_back(std::get <string>(deserialize_attribute(base_type, itr)));
                }
                return vec;

            }
        }

        if (type == "int8") {
            return (int8_t) zigzagDecode(unsignedFromVarintBytes(itr));
        } else if (type == "int16") {
            return (int16_t) zigzagDecode(unsignedFromVarintBytes(itr));
        } else if (type == "int32") {
            return (int32_t) zigzagDecode(unsignedFromVarintBytes(itr));
        } else if (type == "int64") {
            return (int64_t) zigzagDecode(unsignedFromVarintBytes(itr));

        } else if (type == "uint8") {
            return (uint8_t) unsignedFromVarintBytes(itr);
        } else if (type == "uint16") {
            return (uint16_t) unsignedFromVarintBytes(itr);
        } else if (type == "uint32") {
            return (uint32_t) unsignedFromVarintBytes(itr);
        } else if (type == "uint64") {
            return (uint64_t) unsignedFromVarintBytes(itr);

        } else if (type == "fixed8") {
            return (uint8_t) unsignedFromIntBytes(itr, 1);
        } else if (type == "fixed16") {
            return (uint16_t) unsignedFromIntBytes(itr, 2);
        } else if (type == "fixed32") {
            return (uint32_t) unsignedFromIntBytes(itr, 4);
        } else if (type == "fixed64") {
            return (uint64_t) unsignedFromIntBytes(itr, 8);

        } else if (type == "float") {
            uint8_t array_repr[4];
            for (uint8_t &i : array_repr) {
                i = *itr;
                itr++;
            }
            auto *val = reinterpret_cast<float *>(&array_repr);
            return *val;

        } else if (type == "double") {
            uint8_t array_repr[8];
            for (uint8_t &i : array_repr) {
                i = *itr;
                itr++;
            }
            auto *val = reinterpret_cast<double *>(&array_repr);
            return *val;

        } else if (type == "string" || type == "image") {
            uint64_t string_length = unsignedFromVarintBytes(itr);
            string text(itr, itr + string_length);

            itr += string_length;
            return text;

        } else if (type == "ipfs") {
            uint64_t array_length = unsignedFromVarintBytes(itr);
            vector <uint8_t> byte_array = {};
            byte_array.insert(byte_array.begin(), itr, itr + array_length);

            itr += array_length;
            return EncodeBase58(byte_array);

        } else if (type == "bool" || type == "byte") {
            uint8_t next_byte = *itr;
            itr++;
            return next_byte;

        } else {
            check(false, "No type could be matched - " + type);
            return ""; //This point can never be reached because the check above will always throw.
            //Just to silence the compiler warning
        }
    }


    vector <uint8_t> serialize(ATTRIBUTE_MAP attr_map, const vector <FORMAT> &format_lines) {
        uint64_t number = 0;
        vector <uint8_t> serialized_data = {};
        for (FORMAT line : format_lines) {
            auto attribute_itr = attr_map.find(line.name);
            if (attribute_itr != attr_map.end()) {
                const vector <uint8_t> &identifier = toVarintBytes(number + RESERVED);
                serialized_data.insert(serialized_data.end(), identifier.begin(), identifier.end());

                const vector <uint8_t> &child_data = serialize_attribute(line.type, attribute_itr->second);
                serialized_data.insert(serialized_data.end(), child_data.begin(), child_data.end());

                attr_map.erase(attribute_itr);
            }
            number++;
        }
        if (attr_map.begin() != attr_map.end()) {
            check(false,
                "The following attribute could not be serialized, because it is not specified in the provided format: "
                + attr_map.begin()->first);
        }
        return serialized_data;
    }


    ATTRIBUTE_MAP deserialize(const vector <uint8_t> &data, const vector <FORMAT> &format_lines) {
        ATTRIBUTE_MAP attr_map = {};

        auto itr = data.begin();
        while (itr != data.end()) {
            uint64_t identifier = unsignedFromVarintBytes(itr);
            FORMAT format = format_lines.at(identifier - RESERVED);
            attr_map[format.name] = deserialize_attribute(format.type, itr);
        }

        return attr_map;
    }
}