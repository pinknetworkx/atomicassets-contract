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
  vector<uint64_t> asset_ids,
  string memo,
  name scope_payer
);

ACTION logmint(
  name minter,
  uint64_t asset_id,
  uint32_t preset_id,
  name new_owner
);

ACTION setassetdata(
  name editor,
  uint64_t asset_id,
  name new_owner
);

ACTION burnasset(
  name owner,
  uint64_t asset_id
);

ACTION logbackasset(
  name owner,
  uint64_t asset_id,
  asset back_quantity;
);

ACTION lognewpreset(
  uint32_t preset_id;
  name authorized_creator,
  name scheme_name,
  name collection_name,
  bool transferable,
  bool burnable,
  uint64_t max_supply,
  ATTRIBUTE_MAP immutable_data,
  ATTRIBUTE_MAP mutable_data
);

ACTION setpredata(
  name authorized_editor,
  uint32_t preset_id,
  ATTRIBUTE_MAP new_mutable_data
);

*/

CONTRACT atomicassets : public contract {
  public:
    using contract::contract;

    ACTION init();
    ACTION admincoledit(vector<FORMAT> collection_format_extension);
    ACTION setversion(string new_version);

    ACTION transfer(
      name from,
      name to,
      vector<uint64_t> asset_ids,
      string memo
    );
    
    ACTION createscheme(
      name author,
      name scheme_name,
      vector<FORMAT> scheme_format
    );
    ACTION extendscheme(
      name scheme_name,
      vector<FORMAT> scheme_format_extension
    );
    
    ACTION createcol(
      name author,
      name collection_name,
      vector<name> authorized_accounts,
      vector<name> notify_accounts,
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

    ACTION createpre(
      name authorized_creator,
      name scheme_name,
      name collection_name,
      bool transferable,
      bool burnable,
      uint64_t max_supply,
      ATTRIBUTE_MAP immutable_data,
      ATTRIBUTE_MAP mutable_data
    );
    ACTION setpredata(
      name authorized_editor,
      uint32_t preset_id,
      ATTRIBUTE_MAP new_mutable_data
    );

    ACTION mintasset(
      name authorized_minter,
      uint32_t preset_id,
      name new_owner,
      ATTRIBUTE_MAP immutable_data,
      ATTRIBUTE_MAP mutable_data
    );
    ACTION setassetdata(
      name authorized_editor,
      name owner,
      uint64_t asset_id,
      ATTRIBUTE_MAP new_mutable_data
    );
    ACTION burnasset(
      name owner,
      uint64_t asset_id
    );

    ACTION createoffer(
      name sender,
      name recipient,
      vector<uint64_t> sender_asset_ids,
      vector<uint64_t> recipient_asset_ids,
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

    [[eosio::on_notify("eosio.token::transfer")]] void receive_token_transfer(name from, name to, asset quantity, string memo);
    


    ACTION logtransfer(
      name collection_name,
      name from,
      name to,
      vector<uint64_t> asset_ids,
      string memo,
      name scope_payer
    );
    ACTION lognewpreset(
      uint32_t preset_id,
      name authorized_creator,
      name scheme_name,
      name collection_name,
      bool transferable,
      bool burnable,
      uint64_t max_supply,
      ATTRIBUTE_MAP immutable_data,
      ATTRIBUTE_MAP mutable_data
    );
    ACTION logmint(
      name minter,
      uint64_t asset_id,
      uint32_t preset_id,
      name new_owner
    );
    ACTION logbackasset(
      name owner,
      uint64_t asset_id,
      asset back_quantity
    );


  private:

    TABLE schemes_s {
      name                scheme_name;
      name                author;
      vector<FORMAT>      format;

      uint64_t primary_key() const { return scheme_name.value; }
    };
    typedef multi_index<name("schemes"), schemes_s> schemes_t;
    

    TABLE collections_s {
      name                collection_name;
      name                author;
      vector<name>        authorized_accounts;
      vector<name>        notify_accounts;
      vector<uint8_t>     serialized_data;

      uint64_t primary_key() const { return collection_name.value; };
    };
    typedef multi_index<name("collections"), collections_s> collections_t;


    TABLE presets_s {
      uint32_t            id;
      name                scheme_name;
      name                collection_name;
      bool                transferable;
      bool                burnable;
      vector<uint8_t>     max_supply;
      vector<uint8_t>     issued_supply;
      vector<uint8_t>     immutable_serialized_data;
      vector<uint8_t>     mutable_serialized_data;

      uint64_t primary_key() const { return uint64_t{id}; }
    };
    typedef multi_index<name("presets"), presets_s> presets_t;

    //Scope: owner
    TABLE assets_s {
      uint64_t            id;
      uint32_t            preset_id;
      name                ram_payer;
      uint64_t            backed_core_amount;
      vector<uint8_t>     immutable_serialized_data;
      vector<uint8_t>     mutable_serialized_data;

      uint64_t primary_key() const { return id; };
    };
    typedef multi_index<name("assets"), assets_s> assets_t;
    

    TABLE offers_s {
      uint64_t            id;
      name                offer_sender;
      name                offer_recipient;
      vector<uint64_t>    sender_asset_ids;
      vector<uint64_t>    recipient_asset_ids;
      string              memo;

      uint64_t primary_key() const { return id; };
      uint64_t by_sender() const { return offer_sender.value; };
      uint64_t by_recipient() const { return offer_recipient.value; };
    };
    typedef multi_index<name("offers"), offers_s,
    indexed_by<name("sender"), const_mem_fun<offers_s, uint64_t, &offers_s::by_sender>>,
    indexed_by<name("recipient"), const_mem_fun<offers_s, uint64_t, &offers_s::by_recipient>>> offers_t;


    TABLE config_s {
      uint64_t            asset_counter = 1099511627780; //2^40
      uint64_t            offer_counter = 0;
      vector<FORMAT> collection_format = {};
    };
    typedef singleton<name("config"), config_s> config_t;
    // https://github.com/EOSIO/eosio.cdt/issues/280
    typedef multi_index<name("config"), config_s> config_t_for_abi;

    TABLE tokenconfigs_s {
      name           standard = name("atomicassets");
      std::string    version = string("0.1.0");
    };
    typedef singleton<name("tokenconfigs"), tokenconfigs_s> tokenconfigs_t;



    schemes_t schemes = schemes_t(get_self(), get_self().value);
    collections_t collections = collections_t(get_self(), get_self().value);
    presets_t presets = presets_t(get_self(), get_self().value);
    offers_t offers = offers_t(get_self(), get_self().value);
    config_t config = config_t(get_self(), get_self().value);
    tokenconfigs_t tokenconfigs = tokenconfigs_t(get_self(), get_self().value);

    
    void internal_transfer(
      name from,
      name to,
      vector<uint64_t> asset_ids,
      string memo,
      name scope_payer
    );

    assets_t get_assets(name acc);

    vector<uint8_t> int_to_byte_vector(uint64_t number);

    uint64_t byte_vector_to_int(vector<uint8_t> bytes);
};
