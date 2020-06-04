#include <atomicdata.hpp>
#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>

using namespace atomicdata;

namespace atomicassets_public {
    struct TOKEN {
        name   token_contract;
        symbol token_symbol;
    };

    TABLE collections_s {
        name            collection_name;
        name            author;
        bool            allow_notify;
        vector<name>    authorized_accounts;
        vector<name>    notify_accounts;
        double          market_fee;
        vector<uint8_t> serialized_data;

        uint64_t primary_key() const { return collection_name.value; };
    };
    typedef multi_index<name("collections"), collections_s> collections_t;

    // Scope: collection_name
    TABLE schemas_s {
        name           schema_name;
        vector<FORMAT> format;

        uint64_t primary_key() const { return schema_name.value; }
    };
    typedef multi_index<name("schemas"), schemas_s> schemas_t;

    // Scope: collection_name
    TABLE templates_s {
        uint32_t        template_id;
        name            schema_name;
        bool            transferable;
        bool            burnable;
        uint32_t        max_supply;
        uint32_t        issued_supply;
        vector<uint8_t> immutable_serialized_data;

        uint64_t primary_key() const { return uint64_t{template_id}; }
    };
    typedef multi_index<name("templates"), templates_s> templates_t;

    // Scope: owner
    TABLE assets_s {
        uint64_t        asset_id;
        name            collection_name;
        name            schema_name;
        int32_t         template_id;
        name            ram_payer;
        vector<asset>   backed_tokens;
        vector<uint8_t> immutable_serialized_data;
        vector<uint8_t> mutable_serialized_data;

        uint64_t primary_key() const { return asset_id; };
    };
    typedef multi_index<name("assets"), assets_s> assets_t;

    TABLE offers_s {
        uint64_t         offer_id;
        name             sender;
        name             recipient;
        vector<uint64_t> sender_asset_ids;
        vector<uint64_t> recipient_asset_ids;
        string           memo;

        uint64_t primary_key() const { return offer_id; };
        uint64_t by_sender() const { return sender.value; };
        uint64_t by_recipient() const { return recipient.value; };
    };
    typedef multi_index<name("offers"), offers_s,
        indexed_by<name("sender"), const_mem_fun<offers_s, uint64_t, &offers_s::by_sender>>,
        indexed_by<name("recipient"), const_mem_fun<offers_s, uint64_t, &offers_s::by_recipient>>>
        offers_t;

    TABLE balances_s {
        name          owner;
        vector<asset> quantities;

        uint64_t primary_key() const { return owner.value; };
    };
    typedef multi_index<name("balances"), balances_s> balances_t;
} // namespace atomicassets_public