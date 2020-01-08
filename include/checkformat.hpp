#include <eosio/eosio.hpp>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

/*
This function checks if a vector of lines, used to describe a format, is valid

Each line must be a valid json string, following this format:
{"name": "<myname>", "type": "<mytype>", "parent": <mynumber>}

For a format to be vlaid, three things are checked:
1. The type attribute has to be a valid type. Valid types are:
    int8/ int16/ int32/ int64
    uint8/ uint16/ uint32/ uint64
    sint8/ sint16/ sint32/ sint64
    fixed8/ fixed16/ fixed32/ fixed64
    float/ double/ string/ ipfs/ bool/ byte
    object{n}   -   where n is an integer number

    or any valid type followed by [] to describe an array
    nested arrays (e.g. uint64[][][]) are allowed

2. The parent referenced has to have been defined in a previous line
    e.g. INVALID:
    {"name": "testattribute", "type": "string", "parent": 1}
    {"name": "testobject", "type": "object{1}", "parent": 0}

    e.g. VALID:
    {"name": "testobject", "type": "object{1}", "parent": 0}
    {"name": "testattribute", "type": "string", "parent": 1}

3. Names need to be unique within any given parent
    e.g. INVALID:
    {"name": "testattribute", "type": "string", "parent": 0}
    {"name": "testattribute", "type": "string", "parent": 0}

    e.g. VALID:
    {"name": "testattribute", "type": "string", "parent": 0}
    {"name": "otherattribute", "type": "string", "parent": 0}

    e.g. VALID:
    {"name": "testattribute", "type": "string", "parent": 0}
    {"name": "testobject", "type": "object{1}", "parent": 0}
    {"name": "testattribute", "type": "string", "parent": 1}


Note: This could all be done a lot cleaner by using regex or similar libraries
      However, using them would bloat up the contract size significantly.
*/
void check_format(vector<string> lines) {


    map<uint64_t, vector<string>> object_attribute_map;
    object_attribute_map[0] = {};

    for (string line : lines) {

        json linejson = json::parse(line);

        check(linejson["name"].is_string(),
        "'name' attribute either not provided or wrong type - " + line);
        check(linejson["type"].is_string(),
        "'type' attribute either not provided or wrong type - " + line);
        check(linejson["parent"].is_number(),
        "'parent' attribute either not provided or wrong type - " + line);

        string name = linejson["name"];
        string type = linejson["type"];
        uint64_t parent = linejson["parent"];

        size_t offset = 0;
        bool is_object = false;
        uint64_t object_number;
        if (type.find("object{") == 0) {
            string working_str = type.substr(7);
            size_t numend;
            object_number = stoll(working_str, &numend);
            check(numend < working_str.length() && working_str.at(numend) == '}',
                  "'type' attribute has an invalid format - " + line);
            offset = 7 + numend + 1;
            is_object = true;
        } else {
            bool found_num_type = false;
            if (type.find("int", offset) == offset) {
                offset += 3;
                found_num_type = true;
            } else if (type.find("sint", offset) == offset || type.find("uint", offset) == offset) {
                offset += 4;
                found_num_type = true;
            } else if (type.find("fixed", offset) == offset) {
                offset += 5;
                found_num_type = true;
            }
            if (found_num_type) {
                size_t bit_end;
                if (type.find("8", offset) == offset) {
                    offset += 1;
                } else if (type.find("16", offset) == offset || type.find("32", offset) == offset ||
                           type.find("64", offset) == offset) {
                    offset += 2;
                } else {
                    check(false, "'type' attribute has an invalid format - " + line);
                }
            } else {
                if (type.find("bool", offset) == offset || type.find("ipfs", offset) == offset) {
                    offset += 4;
                } else if (type.find("bytes", offset) == offset || type.find("float", offset) == offset) {
                    offset += 5;
                } else if (type.find("string", offset) == offset || type.find("double", offset) == offset) {
                    offset += 6;
                } else {
                    check(false, "'type' attribute has an invalid format - " + line);
                }
            }
        }
        while (offset != type.length()) {
            check(type.find("[]", offset) == offset, "'type' attribute has an invalid format - " + line);
            offset += 2;
        }


        if (is_object) {
            check(object_attribute_map.find(object_number) == object_attribute_map.end(),
                  "objects can only be defined once - " + line);
            object_attribute_map[object_number] = {};
        }

        check(object_attribute_map.find(parent) != object_attribute_map.end(),
                "parent has not been defined - " + line);

        vector<string>& parent_attributes = object_attribute_map[parent];
        check(std::find(parent_attributes.begin(), parent_attributes.end(), name) == parent_attributes.end(),
                "the parent already has an attribute with the same name - " + line);

        parent_attributes.push_back(name);
    }
}