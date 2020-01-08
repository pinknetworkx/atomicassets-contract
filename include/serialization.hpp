#include <eosio/eosio.hpp>
#include <math.h>

#include <nlohmann/json.hpp>
#include "base58.hpp"

using json = nlohmann::json;

using namespace std;

static constexpr uint64_t RESERVED = 4;


vector<uint8_t> toVarintBytes(uint64_t number, uint64_t original_bytes = 8) {
    if (original_bytes < 8) {
        uint64_t bitmask = pow(2, original_bytes * 8) - 1;
        number &= bitmask;
    }

    vector<uint8_t> bytes = {};
    for (uint64_t i = 0; i < original_bytes + 1; i++) {
        if (i == original_bytes) {
            bytes.push_back((uint8_t) number % 256);
        } else {
            if (number >= 128) {
                bytes.push_back((uint8_t) (128 + number % 128));
                number /= 128;
            } else {
                bytes.push_back((uint8_t) number);
                break;
            }
        }
    }

    return bytes;
}

uint64_t unsignedFromVarintBytes(vector<uint8_t>::iterator& itr, uint64_t original_bytes = 8) {
    uint64_t number = 0;
    uint64_t multiplier = 1;
    for (uint64_t i = 0; i < original_bytes + 1; i++) {
        if (i == original_bytes) {
            number += ((uint64_t) *itr) * multiplier;
            itr++;
        } else {
            if (*itr >= 128) {
                number += (((uint64_t) *itr) - 128) * multiplier;
                itr++;
            } else {
                number += ((uint64_t) *itr) * multiplier;
                itr++;
                break;
            }
        }
        multiplier *= 128;
    }

    return number;
}

int64_t signedFromVarintBytes(vector<uint8_t>::iterator& itr, uint64_t original_bytes = 8) {
    uint64_t number = unsignedFromVarintBytes(itr, original_bytes);
    if (original_bytes != 8 && number & (1 << (original_bytes * 8 - 1))) {
        //The number should be negative
        uint64_t mask = 0 - pow(2, original_bytes * 8);
        number += mask;
    }

    return number;
}

//It is expected that the number is smaller than 2^byte_amount
vector<uint8_t> toIntBytes(uint64_t number, uint64_t byte_amount) {
    vector<uint8_t> bytes = {};
    for (uint64_t i = 0; i < byte_amount; i++) {
        bytes.push_back((uint8_t) number % 256);
        number /= 256;
    }
    return bytes;
}

uint64_t unsignedFromIntBytes(vector<uint8_t>::iterator& itr, uint64_t original_bytes = 8) {
    uint64_t number = 0;
    uint64_t multiplier = 1;
    for (uint64_t i = 0; i < original_bytes; i++) {
        number += ((uint64_t) *itr) * multiplier;
        multiplier *= 256;
        itr++;
    }

    return number;
}

int64_t signedFromIntBytes(vector<uint8_t>::iterator& itr, uint64_t original_bytes = 8) {
    uint64_t number = unsignedFromIntBytes(itr, original_bytes);
    if (original_bytes != 8 && number & (1 << (original_bytes * 8 - 1))) {
        //The number should be negative
        uint64_t mask = 0 - (pow(2, original_bytes * 8) - 1);
        number += mask;
    }

    return number;
}



class Attribute {
public:
    uint64_t number;
    string name;
    string type;
    vector<Attribute *> children;

    Attribute(uint64_t _number, string _name, string _type) {
        number = _number;
        name = _name;
        type = _type;
        children = {};
    }

    Attribute(uint64_t _number, string _name, string _type, vector<Attribute *> _children) {
        number = _number;
        name = _name;
        type = _type;
        children = _children;
    }

    vector<uint8_t> serialize(json object) {
        if (type.find("[]", type.length() - 2) == type.length() - 2) {
            //Type is an array
            check(object.is_array(), "Expected an array, but got something else - " + object.dump());
            Attribute attribute_without_array = Attribute(number, name, type.substr(0, type.length() - 2), children);
            vector<uint8_t> serialized_data = toVarintBytes(object.size());
            for (json& element : object) {
                vector<uint8_t> serialized_element = attribute_without_array.serialize(element);
                serialized_data.insert(serialized_data.end(), serialized_element.begin(), serialized_element.end());
            }
            return serialized_data;
        }

        if (type.find("object") == 0) {
            //Type is an object
            check(object.is_object(), "Expected an object, but got something else - " + object.dump());
            vector<uint8_t> serialized_data = {};
            for (Attribute *child : children) {
                if (!object[child->name].is_null()) {
                    vector<uint8_t> identifier = toVarintBytes(child->number + RESERVED);
                    serialized_data.insert(serialized_data.end(), identifier.begin(), identifier.end());
                    vector<uint8_t> child_data = child->serialize(object[child->name]);
                    serialized_data.insert(serialized_data.end(), child_data.begin(), child_data.end());
                }
            }
            serialized_data.push_back((uint8_t) 0);
            return serialized_data;
        }

        if (type.find("int") == 0) {
            check(object.type() == json::value_t::number_integer || object.type() == json::value_t::number_unsigned,
                  "Expected an integer, but got something else - " + object.dump());
            uint64_t bits = stoll(type.substr(3));
            check(bits == 64 || (object < pow(2, bits - 1) && object >= -1 * pow(2, bits - 1)),
                  "The number exceeds the value range for this type - " + object.dump());
            return toVarintBytes(object, bits / 8);

        } else if (type.find("uint") == 0) {
            check(object.type() == json::value_t::number_unsigned,
                  "Expected an unsigned integer, but got something else - " + object.dump());
            uint64_t bits = stoll(type.substr(4));
            check(bits == 64 || object < pow(2, bits),
                  "The number exceeds the max value for this type - " + object.dump());
            return toVarintBytes(object, bits / 8);

        } else if (type.find("sint") == 0) {
            check(object.type() == json::value_t::number_integer || object.type() == json::value_t::number_unsigned,
                  "Expected an integer, but got something else - " + object.dump());
            uint64_t bits = stoll(type.substr(4));
            check(bits == 64 || (object <= pow(2, bits - 1) && object > -1 * pow(2, bits - 1)),
                  "The number exceeds the value range for this type - " + object.dump());
            return toVarintBytes(-1 * (int64_t) object, bits / 8);

        } else if (type.find("fixed") == 0) {
            check(object.type() == json::value_t::number_integer || object.type() == json::value_t::number_unsigned,
                  "Expected an integer, but got something else - " + object.dump());
            uint64_t bits = stoll(type.substr(5));
            check(bits == 64 || (object < pow(2, bits - 1) && object >= -1 * pow(2, bits - 1)),
                  "The number exceeds the value range for this type - " + object.dump());
            return toIntBytes((int64_t) object, bits / 8);

        } else if (type.compare("float") == 0) {
            check(object.type() == json::value_t::number_float,
                  "Expected an floating point number, but got something else");
            float float_value = object;
            uint8_t* byte_value = reinterpret_cast<uint8_t*>(&float_value);
            vector<uint8_t> serialized_data = {};
            for (int i = 0; i < 4; i++) {
                serialized_data.push_back(*(byte_value + i));
            }
            return serialized_data;

        } else if (type.compare("double") == 0) {
            check(object.type() == json::value_t::number_float,
                  "Expected an floating point number, but got something else - " + object.dump());
            double float_value = object;
            uint8_t* byte_value = reinterpret_cast<uint8_t*>(&float_value);
            vector<uint8_t> serialized_data = {};
            for (int i = 0; i < 8; i++) {
                serialized_data.push_back(*(byte_value + i));
            }
            return serialized_data;

        } else if (type.compare("string") == 0) {
            check(object.is_string(), "Expected a string, but got something else - " + object.dump());
            string text = object;
            vector<uint8_t> serialized_data(text.begin(), text.end());

            vector<uint8_t> length_bytes = toVarintBytes(text.length());
            serialized_data.insert(serialized_data.begin(), length_bytes.begin(), length_bytes.end());
            return serialized_data;

        } else if (type.compare("ipfs") == 0) {
            check(object.is_string(), "Expected a string, but got something else - " + object.dump());
            vector<uint8_t> result = {};
            DecodeBase58((string) object, result);
            vector<uint8_t> length_bytes = toVarintBytes(result.size());
            result.insert(result.begin(), length_bytes.begin(), length_bytes.end());
            return result;

        } else if (type.compare("bool") == 0) {
            check(object.is_boolean(), "Expected a bool, but got something else - " + object.dump());
            if ((bool) object) {
                return {1};
            } else {
                return {0};
            }

        } else if (type.compare("byte") == 0) {
            check(object.type() == json::value_t::number_unsigned,
                  "Expected a byte represented as an unsigned integer, but got something else - " + object.dump());
            uint64_t bits = stoll(type.substr(4));
            check(object < 256,
                  "The number exceeds the max value for this type - " + object.dump());
            return {(uint8_t) object};
        } else {
            check(false, "No type could be matched - " + type);
            return {}; //This point can never be reached because the check above will always throw.
            //Just to silence the compiler warning
        }


    }


    //it is assumed that the data passed to this function is valid
    json deserialize(vector<uint8_t>::iterator& itr) {
        if (type.find("[]", type.length() - 2) == type.length() - 2) {
            //Type is an array
            json array = json::array();
            Attribute attribute_without_array = Attribute(number, name, type.substr(0, type.length() - 2), children);
            uint64_t array_length = unsignedFromVarintBytes(itr);
            for (uint64_t i = 0; i < array_length; i++) {
                array.push_back(attribute_without_array.deserialize(itr));
            }
            return array;
        }

        if (type.find("object") == 0) {
            //Type is an object
            json object = json::object();
            while (true) {
                uint64_t identifier = unsignedFromVarintBytes(itr);
                if (!identifier) {
                    break;
                }
                uint64_t child_number = identifier - RESERVED;
                Attribute* child = children.at(child_number);
                object[child->name] = child->deserialize(itr);
            }
            return object;
        }

        if (type.find("int") == 0) {
            uint64_t bits = stoll(type.substr(3));
            return signedFromVarintBytes(itr, bits / 8);

        } else if (type.find("uint") == 0) {
            uint64_t bits = stoll(type.substr(4));
            return unsignedFromVarintBytes(itr, bits / 8);

        } else if (type.find("sint") == 0) {
            uint64_t bits = stoll(type.substr(4));
            return -1 * signedFromVarintBytes(itr, bits / 8);

        } else if (type.find("fixed") == 0) {
            uint64_t bits = stoll(type.substr(5));
            return unsignedFromIntBytes(itr, bits / 8);

        } else if (type.compare("float") == 0) {
            uint8_t array_repr[4];
            for (uint64_t i = 0; i < 4; i++) {
                array_repr[i] = *itr;
                itr++;
            }
            float* val = reinterpret_cast<float*>(&array_repr);
            return *val;

        } else if (type.compare("double") == 0) {
            uint8_t array_repr[8];
            for (uint64_t i = 0; i < 8; i++) {
                array_repr[i] = *itr;
                itr++;
            }
            double* val = reinterpret_cast<double*>(&array_repr);
            return *val;

        } else if (type.compare("string") == 0) {
            Attribute char_array_attribute = Attribute(number, name, "fixed8[]");
            vector<char> char_array = char_array_attribute.deserialize(itr);

            string text(char_array.begin(), char_array.end());
            return text;

        } else if (type.compare("ipfs") == 0) {
            Attribute byte_array_attribute = Attribute(number, name, "fixed8[]");
            json byte_array = byte_array_attribute.deserialize(itr);
            //Arrays are stored as vectors in the json library
            return EncodeBase58(byte_array);

        } else if (type.compare("bool") == 0) {
            uint8_t next_byte = *itr;
            itr++;
            return next_byte == 1;

        } else if (type.compare("byte") == 0) {
            uint8_t next_byte = *itr;
            itr++;
            return next_byte;

        } else {
            check(false, "No type could be matched - " + type);
            return ""; //This point can never be reached because the check above will always throw.
            //Just to silence the compiler warning
        }
    }

    json deserialize(vector<uint8_t> bytes) {
        auto itr = bytes.begin();
        return deserialize(itr);
    }

};


bool isObjectType(const string &type) {
    return type.find("object") == 0;
}

uint64_t getObjectNumber(const string &type) {
    return stoll(type.substr(7));
}


Attribute buildRootAttribute(const vector<string>& format) {
    map<uint64_t, Attribute *> object_attributes;
    Attribute root = Attribute(0, "root", "object{0}");
    object_attributes[0] = &root;

    for (string line : format) {
        json linejson = json::parse(line);

        string name = linejson["name"];
        string type = linejson["type"];
        uint64_t parent = linejson["parent"];

        Attribute *parent_attribute = object_attributes[parent];
        Attribute *child_attribute = new Attribute(parent_attribute->children.size(), name, type);
        parent_attribute->children.push_back(child_attribute);
        if (isObjectType(type)) {
            object_attributes[getObjectNumber(type)] = child_attribute;
        }
    }

    return root;
}


vector<uint8_t> serialize(const string& data, const vector<string>& format) {
    if (data.size() == 0) {
        return {};
    }
    //First, building a format object
    map<uint64_t, Attribute *> object_attributes;
    Attribute root = buildRootAttribute(format);

    vector<uint8_t> serialized_data = root.serialize(json::parse(data));
    serialized_data.pop_back(); //The last 0 byte (closing the root object) is redundant
    return serialized_data;
}


json deserialize_to_json(const vector<uint8_t>& bytes, const vector<string>& format) {
    //First, building a format object
    map<uint64_t, Attribute *> object_attributes;
    Attribute root = buildRootAttribute(format);

    vector<uint8_t> modifiable_bytes = bytes;
    modifiable_bytes.push_back(0); //Tha last 0 byte (closing the root object) is not stored, but required for a successful deserialization
    return root.deserialize(modifiable_bytes);
}


string deserialize_to_string(const vector<uint8_t>& bytes, const vector<string>& format) {
    return deserialize_to_json(bytes, format).dump();
}