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
  name collection_name,
  name scheme_name,
  int32_t preset_id,
  name new_owner
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

ACTION logbackasset(
  name owner,
  uint64_t asset_id,
  asset back_quantity;
);

ACTION lognewpreset(
  int32_t preset_id;
  name authorized_creator,
  name collection_name,
  name scheme_name,
  bool transferable,
  bool burnable,
  uint32_t max_supply,
  ATTRIBUTE_MAP immutable_data
);

*/

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
      vector<uint64_t> asset_ids,
      string memo
    );
    
    ACTION createcol(
      name author,
      name collection_name,
      bool allow_notify,
      vector<name> authorized_accounts,
      vector<name> notify_accounts,
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

    ACTION createscheme(
      name authorized_creator,
      name collection_name,
      name scheme_name,
      vector<FORMAT> scheme_format
    );
    ACTION extendscheme(
      name authorized_editor,
      name collection_name,
      name scheme_name,
      vector<FORMAT> scheme_format_extension
    );

    ACTION createpreset(
      name authorized_creator,
      name collection_name,
      name scheme_name,
      bool transferable,
      bool burnable,
      uint32_t max_supply,
      ATTRIBUTE_MAP immutable_data
    );

    ACTION mintasset(
      name authorized_minter,
      name collection_name,
      name scheme_name,
      int32_t preset_id,
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
    ACTION backsymbol(
      name ram_payer,
      name owner,
      uint64_t asset_id,
      symbol symbol_to_announce
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

    void receive_token_transfer(name from, name to, asset quantity, string memo);
    


    ACTION logtransfer(
      name collection_name,
      name from,
      name to,
      vector<uint64_t> asset_ids,
      string memo,
      name scope_payer
    );
    ACTION lognewpreset(
      int32_t preset_id,
      name authorized_creator,
      name scheme_name,
      name collection_name,
      bool transferable,
      bool burnable,
      uint32_t max_supply,
      ATTRIBUTE_MAP immutable_data
    );
    ACTION logmint(
      name minter,
      uint64_t asset_id,
      name collection_name,
      name scheme_name,
      int32_t preset_id,
      name new_owner
    );
    ACTION logbackasset(
      name owner,
      uint64_t asset_id,
      asset back_quantity
    );


  private:

    struct TOKEN {
      name token_contract;
      symbol token_symbol;
    };

    TABLE collections_s {
      name                collection_name;
      name                author;
      bool                allow_notify;
      vector<name>        authorized_accounts;
      vector<name>        notify_accounts;
      double              market_fee;
      vector<uint8_t>     serialized_data;

      uint64_t primary_key() const { return collection_name.value; };
    };
    typedef multi_index<name("collections"), collections_s> collections_t;


    //Scope: collection_name
    TABLE schemes_s {
      name                scheme_name;
      vector<FORMAT>      format;

      uint64_t primary_key() const { return scheme_name.value; }
    };
    typedef multi_index<name("schemes"), schemes_s> schemes_t;


    TABLE presets_s {
      uint32_t            preset_id;
      name                collection_name;
      name                scheme_name;
      bool                transferable;
      bool                burnable;
      uint32_t            max_supply;
      uint32_t            issued_supply;
      vector<uint8_t>     immutable_serialized_data;

      uint64_t primary_key() const { return uint64_t{preset_id}; }
    };
    typedef multi_index<name("presets"), presets_s> presets_t;


    //Scope: owner
    TABLE assets_s {
      uint64_t            asset_id;
      name                collection_name;
      name                scheme_name;
      int32_t             preset_id;
      name                ram_payer;
      vector<asset>       backed_tokens;
      vector<uint8_t>     immutable_serialized_data;
      vector<uint8_t>     mutable_serialized_data;

      uint64_t primary_key() const { return asset_id; };
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
      vector<FORMAT>      collection_format = {};
      vector<TOKEN>       supported_tokens = {};
    };
    typedef singleton<name("config"), config_s> config_t;
    // https://github.com/EOSIO/eosio.cdt/issues/280
    typedef multi_index<name("config"), config_s> config_t_for_abi;

    TABLE tokenconfigs_s {
      name           standard = name("atomicassets");
      std::string    version = string("0.2.0");
    };
    typedef singleton<name("tokenconfigs"), tokenconfigs_s> tokenconfigs_t;
    // https://github.com/EOSIO/eosio.cdt/issues/280
    typedef multi_index<name("tokenconfigs"), tokenconfigs_s> tokenconfigs_t_for_abi;



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

    schemes_t get_schemes(name collection_name);
};

extern "C"
void apply(uint64_t receiver, uint64_t code, uint64_t action)
{
	if (code == receiver) {
		switch (action) {
      EOSIO_DISPATCH_HELPER(atomicassets, \
      (init)(admincoledit)(setversion)(addconftoken)(transfer) \
      (createcol)(setcoldata)(addcolauth)(remcolauth)(addnotifyacc)(remnotifyacc) \
      (setmarketfee)(forbidnotify)(createscheme)(extendscheme)(createpreset) \
      (mintasset)(setassetdata)(backsymbol)(burnasset) \
      (createoffer)(canceloffer)(acceptoffer)(declineoffer))
		}
	} else if (action == name("transfer").value) {
    eosio::execute_action(name(receiver), name(code), &atomicassets::receive_token_transfer);
  }
}
