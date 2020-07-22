/*

This file is not used for the actual atomicassets contract.
It can be used as a header file for other contracts to access the atomicassets tables
and custom data types.

*/


#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

namespace atomicassets {
    static constexpr double MAX_MARKET_FEE = 0.15;

    static constexpr name ATOMICASSETS_ACCOUNT = name("atomicassets");

    //Custom vector types need to be defined because otherwise a bug in the ABI serialization
    //would cause the ABI to be invalid
    typedef std::vector <int8_t>      INT8_VEC;
    typedef std::vector <int16_t>     INT16_VEC;
    typedef std::vector <int32_t>     INT32_VEC;
    typedef std::vector <int64_t>     INT64_VEC;
    typedef std::vector <uint8_t>     UINT8_VEC;
    typedef std::vector <uint16_t>    UINT16_VEC;
    typedef std::vector <uint32_t>    UINT32_VEC;
    typedef std::vector <uint64_t>    UINT64_VEC;
    typedef std::vector <float>       FLOAT_VEC;
    typedef std::vector <double>      DOUBLE_VEC;
    typedef std::vector <std::string> STRING_VEC;

    typedef std::variant <\
        int8_t, int16_t, int32_t, int64_t, \
        uint8_t, uint16_t, uint32_t, uint64_t, \
        float, double, std::string, \
        INT8_VEC, INT16_VEC, INT32_VEC, INT64_VEC, \
        UINT8_VEC, UINT16_VEC, UINT32_VEC, UINT64_VEC, \
        FLOAT_VEC, DOUBLE_VEC, STRING_VEC
    > ATOMIC_ATTRIBUTE;

    typedef std::map <std::string, ATOMIC_ATTRIBUTE> ATTRIBUTE_MAP;

    struct FORMAT {
        string name;
        string type;
    };

    struct collections_s {
        name             collection_name;
        name             author;
        bool             allow_notify;
        vector <name>    authorized_accounts;
        vector <name>    notify_accounts;
        double           market_fee;
        vector <uint8_t> serialized_data;

        uint64_t primary_key() const { return collection_name.value; };
    };

    typedef multi_index <name("collections"), collections_s> collections_t;


    //Scope: collection_name
    struct schemas_s {
        name            schema_name;
        vector <FORMAT> format;

        uint64_t primary_key() const { return schema_name.value; }
    };

    typedef multi_index <name("schemas"), schemas_s> schemas_t;


    //Scope: collection_name
    struct templates_s {
        int32_t          template_id;
        name             schema_name;
        bool             transferable;
        bool             burnable;
        uint32_t         max_supply;
        uint32_t         issued_supply;
        vector <uint8_t> immutable_serialized_data;

        uint64_t primary_key() const { return (uint64_t) template_id; }
    };

    typedef multi_index <name("templates"), templates_s> templates_t;


    //Scope: owner
    struct assets_s {
        uint64_t         asset_id;
        name             collection_name;
        name             schema_name;
        int32_t          template_id;
        name             ram_payer;
        vector <asset>   backed_tokens;
        vector <uint8_t> immutable_serialized_data;
        vector <uint8_t> mutable_serialized_data;

        uint64_t primary_key() const { return asset_id; };
    };

    typedef multi_index <name("assets"), assets_s> assets_t;


    struct offers_s {
        uint64_t          offer_id;
        name              sender;
        name              recipient;
        vector <uint64_t> sender_asset_ids;
        vector <uint64_t> recipient_asset_ids;
        string            memo;
        name              ram_payer;

        uint64_t primary_key() const { return offer_id; };

        uint64_t by_sender() const { return sender.value; };

        uint64_t by_recipient() const { return recipient.value; };
    };

    typedef multi_index <name("offers"), offers_s,
        indexed_by < name("sender"), const_mem_fun < offers_s, uint64_t, &offers_s::by_sender>>,
    indexed_by <name("recipient"), const_mem_fun < offers_s, uint64_t, &offers_s::by_recipient>>>
    offers_t;

    struct balances_s {
        name           owner;
        vector <asset> quantities;

        uint64_t primary_key() const { return owner.value; };
    };

    typedef multi_index <name("balances"), balances_s>       balances_t;


    struct config_s {
        uint64_t                 asset_counter     = 1099511627776; //2^40
        int32_t                  template_counter  = 1;
        uint64_t                 offer_counter     = 1;
        vector <FORMAT>          collection_format = {};
        vector <extended_symbol> supported_tokens  = {};
    };
    typedef singleton <name("config"), config_s>             config_t;

    struct tokenconfigs_s {
        name        standard = name("atomicassets");
        std::string version  = string("1.2.1");
    };
    typedef singleton <name("tokenconfigs"), tokenconfigs_s> tokenconfigs_t;


    collections_t  collections  = collections_t(ATOMICASSETS_ACCOUNT, ATOMICASSETS_ACCOUNT.value);
    offers_t       offers       = offers_t(ATOMICASSETS_ACCOUNT, ATOMICASSETS_ACCOUNT.value);
    balances_t     balances     = balances_t(ATOMICASSETS_ACCOUNT, ATOMICASSETS_ACCOUNT.value);
    config_t       config       = config_t(ATOMICASSETS_ACCOUNT, ATOMICASSETS_ACCOUNT.value);
    tokenconfigs_t tokenconfigs = tokenconfigs_t(ATOMICASSETS_ACCOUNT, ATOMICASSETS_ACCOUNT.value);

    assets_t get_assets(name acc) {
        return assets_t(ATOMICASSETS_ACCOUNT, acc.value);
    }

    schemas_t get_schemas(name collection_name) {
        return schemas_t(ATOMICASSETS_ACCOUNT, collection_name.value);
    }

    templates_t get_templates(name collection_name) {
        return templates_t(ATOMICASSETS_ACCOUNT, collection_name.value);
    }
};