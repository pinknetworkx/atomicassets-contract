#include "atomicassets_public.hpp"
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

#include <checkformat.hpp>
#include <atomicdata.hpp>

using namespace eosio;
using namespace std;
using namespace atomicdata;

/*
Nofification Receipts for notify_accounts in a collection


    ACTION logtransfer(
        name collection_name,
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );

    ACTION logmint(
        uint64_t asset_id,
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data,
        vector <asset> backed_tokens
    );

    ACTION logsetdata(
        name asset_owner,
        uint64_t asset_id,
        ATTRIBUTE_MAP old_data,
        ATTRIBUTE_MAP new_data
    );

    ACTION logbackasset(
        name asset_owner,
        uint64_t asset_id,
        asset backed_token
    );

    ACTION logburnasset(
        name asset_owner,
        uint64_t asset_id,
        name collection_name,
        name schema_name,
        int32_t template_id,
        vector <asset> backed_tokens,
        ATTRIBUTE_MAP old_immutable_data,
        ATTRIBUTE_MAP old_mutable_data,
        name asset_ram_payer
    );

    ACTION lognewtempl(
        int32_t template_id,
        name authorized_creator,
        name schema_name,
        name collection_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP immutable_data
    );

*/
using namespace atomicassets_public;

CONTRACT atomicassets : public contract {
  public:
    using contract::contract;

    ACTION init();

    ACTION admincoledit(vector<FORMAT> collection_format_extension);

    ACTION setversion(string new_version);

    ACTION addconftoken(name token_contract, symbol token_symbol);


    ACTION transfer(
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );


    ACTION createcol(
        name author,
        name collection_name,
        bool allow_notify,
        vector <name> authorized_accounts,
        vector <name> notify_accounts,
        double market_fee,
        ATTRIBUTE_MAP data
    );

    ACTION setcoldata(
        name collection_name,
        ATTRIBUTE_MAP data
    );

    ACTION addcolauth(
        name collection_name,
        name account_to_add
    );

    ACTION remcolauth(
        name collection_name,
        name account_to_remove
    );

    ACTION addnotifyacc(
        name collection_name,
        name account_to_add
    );

    ACTION remnotifyacc(
        name collection_name,
        name account_to_remove
    );

    ACTION setmarketfee(
        name collection_name,
        double market_fee
    );

    ACTION forbidnotify(
        name collection_name
    );


    ACTION createschema(
        name authorized_creator,
        name collection_name,
        name schema_name,
        vector <FORMAT> schema_format
    );

    ACTION extendschema(
        name authorized_editor,
        name collection_name,
        name schema_name,
        vector <FORMAT> schema_format_extension
    );


    ACTION createtempl(
        name authorized_creator,
        name collection_name,
        name schema_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP immutable_data
    );

    ACTION locktemplate(
        name authorized_editor,
        name collection_name,
        uint32_t template_id
    );


    ACTION mintasset(
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data,
        vector <asset> tokens_to_back
    );

    ACTION setassetdata(
        name authorized_editor,
        name asset_owner,
        uint64_t asset_id,
        ATTRIBUTE_MAP new_mutable_data
    );


    ACTION announcedepo(
        name owner,
        symbol symbol_to_announce
    );

    ACTION withdraw(
        name owner,
        asset token_to_withdraw
    );

    ACTION backasset(
        name payer,
        name asset_owner,
        uint64_t asset_id,
        asset token_to_back
    );

    ACTION burnasset(
        name asset_owner,
        uint64_t asset_id
    );


    ACTION createoffer(
        name sender,
        name recipient,
        vector <uint64_t> sender_asset_ids,
        vector <uint64_t> recipient_asset_ids,
        string memo
    );

    ACTION canceloffer(
        uint64_t offer_id
    );

    ACTION acceptoffer(
        uint64_t offer_id
    );

    ACTION declineoffer(
        uint64_t offer_id
    );

    ACTION payofferram(
        name payer,
        uint64_t offer_id
    );


    void receive_token_transfer(name from, name to, asset quantity, string memo);


    ACTION logtransfer(
        name collection_name,
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo
    );

    ACTION lognewoffer(
        uint64_t offer_id,
        name sender,
        name recipient,
        vector <uint64_t> sender_asset_ids,
        vector <uint64_t> recipient_asset_ids,
        string memo
    );

    ACTION lognewtempl(
        int32_t template_id,
        name authorized_creator,
        name schema_name,
        name collection_name,
        bool transferable,
        bool burnable,
        uint32_t max_supply,
        ATTRIBUTE_MAP immutable_data
    );

    ACTION logmint(
        uint64_t asset_id,
        name authorized_minter,
        name collection_name,
        name schema_name,
        int32_t template_id,
        name new_asset_owner,
        ATTRIBUTE_MAP immutable_data,
        ATTRIBUTE_MAP mutable_data,
        vector <asset> backed_tokens
    );

    ACTION logsetdata(
        name asset_owner,
        uint64_t asset_id,
        ATTRIBUTE_MAP old_data,
        ATTRIBUTE_MAP new_data
    );

    ACTION logbackasset(
        name asset_owner,
        uint64_t asset_id,
        asset backed_token
    );

    ACTION logburnasset(
        name asset_owner,
        uint64_t asset_id,
        name collection_name,
        name schema_name,
        int32_t template_id,
        vector <asset> backed_tokens,
        ATTRIBUTE_MAP old_immutable_data,
        ATTRIBUTE_MAP old_mutable_data,
        name asset_ram_payer
    );


  private:
    TABLE config_s {
        uint64_t       asset_counter     = 1099511627776; // 2^40
        int32_t        template_counter  = 1;
        uint64_t       offer_counter     = 1;
        vector<FORMAT> collection_format = {};
        vector<TOKEN>  supported_tokens  = {};
    };
    typedef singleton<name("config"), config_s> config_t;
    // https://github.com/EOSIO/eosio.cdt/issues/280
    typedef multi_index<name("config"), config_s> config_t_for_abi;

    TABLE tokenconfigs_s {
        name        standard = name("atomicassets");
        std::string version  = string("0.2.0");
    };
    typedef singleton<name("tokenconfigs"), tokenconfigs_s> tokenconfigs_t;
    // https://github.com/EOSIO/eosio.cdt/issues/280
    typedef multi_index<name("tokenconfigs"), tokenconfigs_s> tokenconfigs_t_for_abi;


    collections_t  collections  = collections_t(get_self(), get_self().value);
    offers_t       offers       = offers_t(get_self(), get_self().value);
    balances_t     balances     = balances_t(get_self(), get_self().value);
    config_t       config       = config_t(get_self(), get_self().value);
    tokenconfigs_t tokenconfigs = tokenconfigs_t(get_self(), get_self().value);


    void internal_transfer(
        name from,
        name to,
        vector <uint64_t> asset_ids,
        string memo,
        name scope_payer
    );

    void internal_back_asset(
        name ram_payer,
        name asset_owner,
        uint64_t asset_id,
        asset back_quantity
    );

    void internal_decrease_balance(
        name owner,
        asset quantity
    );

    void notify_collection_accounts(
        name collection_name
    );

    void check_has_collection_auth(
        name account_to_check,
        name collection_name,
        string error_message
    );

    void check_name_length(ATTRIBUTE_MAP data);

    assets_t get_assets(name acc);

    schemas_t get_schemas(name collection_name);

    templates_t get_templates(name collection_name);
};

