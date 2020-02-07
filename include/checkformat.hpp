#include <eosio/eosio.hpp>
#include <algorithm>

#include <atomicdata.hpp>

using namespace eosio;
using namespace std;
using namespace atomicdata;

/*
This function checks if a vector of FORMAT structs, used to describe a format, is valid

For a format to be vlaid, three things are checked:
1. The type attribute has to be a valid type. Valid types are:
    int8/ int16/ int32/ int64
    uint8/ uint16/ uint32/ uint64
    fixed8/ fixed16/ fixed32/ fixed64
    float/ double/ string/ ipfs/ bool/ byte

    or any valid type followed by [] to describe a vector
    nested vectors (e.g. uint64[][]) are not allowed

2. Names need to be unique


Note: This could all be done a lot cleaner by using regex or similar libraries
      However, using them would bloat up the contract size significantly.
*/
void check_format(vector<FORMAT> lines) {

    vector<string> attribute_names = {};

    for (FORMAT line : lines) {

        string name = line.name;
        string type = line.type;

        size_t offset = 0;
        bool found_num_type = false;
        if (type.find("int", offset) == offset) {
            offset += 3;
            found_num_type = true;
        } else if (type.find("uint", offset) == offset) {
            offset += 4;
            found_num_type = true;
        } else if (type.find("fixed", offset) == offset) {
            offset += 5;
            found_num_type = true;
        }
        if (found_num_type) {
            if (type.find("8", offset) == offset) {
                offset += 1;
            } else if (type.find("16", offset) == offset || type.find("32", offset) == offset ||
                       type.find("64", offset) == offset) {
                offset += 2;
            } else {
                check(false, "'type' attribute has an invalid format - " + line.type);
            }
        } else {
            if (type.find("bool", offset) == offset || type.find("ipfs", offset) == offset) {
                offset += 4;
            } else if (type.find("bytes", offset) == offset || type.find("float", offset) == offset) {
                offset += 5;
            } else if (type.find("string", offset) == offset || type.find("double", offset) == offset) {
                offset += 6;
            } else {
                check(false, "'type' attribute has an invalid format - " + line.type);
            }
        }
        if (offset != type.length()) {
            check(type.find("[]", offset) == offset, "'type' attribute has an invalid format - " + line.type);
            offset += 2;
        }
        check(offset == type.length(), "'type' attribute has an invalid format - " + line.type);

        check(std::find(attribute_names.begin(), attribute_names.end(), name) == attribute_names.end(),
                "there already is an attribute with the same name - " + line.name);

        attribute_names.push_back(name);
    }
}