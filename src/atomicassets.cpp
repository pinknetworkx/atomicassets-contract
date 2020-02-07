#include <atomicassets.hpp>

static constexpr symbol CORE_SYMBOL = symbol("WAX", 8);
static constexpr uint64_t MAX_ID = 18446744073709551615;

/**
*  Initializes the config table. Only needs to be called once when first deploying the contract
*  @required_auth The contract itself
*/
ACTION atomicassets::init() {
  require_auth(get_self());
  config.get_or_create(get_self(),config_s{});
  tokenconfigs.get_or_create(get_self(), tokenconfigs_s{});
}

/**
*  Adds one or more lines to the format that is used for collection data serialization
*  @required_auth The contract itself
*/
ACTION atomicassets::admincoledit(vector<atomicdata::FORMAT> collection_format_extension) {
  require_auth(get_self());

  check(collection_format_extension.size() != 0, "Need to add at least one new line");

  auto current_config = config.get();
  current_config.collection_format.insert(
    current_config.collection_format.end(),
    collection_format_extension.begin(),
    collection_format_extension.end()
  );
  check_format(current_config.collection_format);
  
  config.set(current_config, get_self());
}


ACTION atomicassets::setversion(string new_version) {
  require_auth(get_self());

  auto current_tokenconfigs = tokenconfigs.get();
  current_tokenconfigs.version = new_version;
  
  tokenconfigs.set(current_tokenconfigs, get_self());
}


/**
*  Transfers one or more assets to another account
*  @required_auth The from account
*/
ACTION atomicassets::transfer(
  name from,
  name to,
  vector<uint64_t> asset_ids,
  string memo
) {
  require_auth(from);
  internal_transfer(from, to, asset_ids, memo, from);
}


/**
*  Creates a new scheme.
*  Schemes can be used freely by anyone, and can only be extended in the future, but never changed retroactively.
*  This guarantees a correct deserialization for existing presets and assets
*/
ACTION atomicassets::createscheme(
  name author,
  name scheme_name,
  vector<FORMAT> scheme_format
) {
  require_auth(author);
  check(schemes.find(scheme_name.value) == schemes.end(),
  "A scheme with this name already exists");
  check(!is_account(scheme_name) || has_auth(scheme_name),
  "You can't create a scheme with a name of an existing, different account");

  check_format(scheme_format);

  schemes.emplace(author, [&](auto& _scheme){
    _scheme.scheme_name = scheme_name;
    _scheme.author = author;
    _scheme.format = scheme_format;
  });
}


/**
*  Adds one or more lines to the format of an existing scheme
*  @required_auth The collection author
*/
ACTION atomicassets::extendscheme(
  name scheme_name,
  vector<FORMAT> scheme_format_extension
) {
  auto scheme_itr = schemes.require_find(scheme_name.value,
  "No scheme with this name exists");
  
  require_auth(scheme_itr->author);

  check(scheme_format_extension.size() != 0, "Need to add at least one new line");

  vector<FORMAT> lines = scheme_itr->format;
  lines.insert(lines.end(), scheme_format_extension.begin(), scheme_format_extension.end());
  check_format(lines);

  schemes.modify(scheme_itr, same_payer, [&](auto& _scheme) {
    _scheme.format = lines;
  });
}


/**
*  Creates a new collection
*/
ACTION atomicassets::createcol(
  name author,
  name collection_name,
  vector<name> authorized_accounts,
  vector<name> notify_accounts,
  ATTRIBUTE_MAP data
) {
  require_auth(author);

  check(!is_account(collection_name) || has_auth(collection_name),
  "You can't create a collection with a name of an existing, different account");

  check(collections.find(collection_name.value) == collections.end(),
  "A collection with this name already exists");

  for (auto itr = authorized_accounts.begin(); itr != authorized_accounts.end(); itr++) {
    check(is_account(*itr), string("At least one account does not exist - " + itr->to_string()).c_str());
    check(std::find(authorized_accounts.begin(), authorized_accounts.end(), *itr) == itr,
    "You can't have duplicates in the authorized_accounts");
  }
  for (auto itr = notify_accounts.begin(); itr != notify_accounts.end(); itr++) {
    check(is_account(*itr), string("At least one account does not exist - " + itr->to_string()).c_str());
    check(std::find(notify_accounts.begin(), notify_accounts.end(), *itr) == itr,
    "You can't have duplicates in the notify_accounts");
  }

  auto current_config = config.get();

  collections.emplace(author, [&](auto& _collection) {
    _collection.collection_name = collection_name;
    _collection.author = author;
    _collection.authorized_accounts = authorized_accounts;
    _collection.notify_accounts = notify_accounts;
    _collection.serialized_data = serialize(data, current_config.collection_format);
  });
}


/**
*  Sets the collection data, which is then serialized with the collection format set in the config
*  This data is used by 3rd party apps and sites to display additional information about the collection
*  @required_auth The collection author
*/
ACTION atomicassets::setcoldata(
  name collection_name,
  ATTRIBUTE_MAP data
) {
  auto collection_itr = collections.require_find(collection_name.value,
  "No collection with this name exists");
  
  require_auth(collection_itr->author);

  auto current_config = config.get();
  collections.modify(collection_itr, same_payer, [&](auto& _collection) {
    _collection.serialized_data = serialize(data, current_config.collection_format);
  });
}


/**
*  Adds an account to the authorized_accounts list of a collection
*  This will allow the account to create and edit both presets and assets that belong to this collection
*  @required_atuh The collection author
*/
ACTION atomicassets::addcolauth(
  name collection_name,
  name account_to_add
) {
  auto collection_itr = collections.require_find(collection_name.value,
  "No collection with this name exists");
  
  require_auth(collection_itr->author);

  check(is_account(account_to_add), "The account does not exist");

  vector<name> authorized_accounts = collection_itr->authorized_accounts;
  check(std::find(authorized_accounts.begin(), authorized_accounts.end(), account_to_add) == authorized_accounts.end(),
  "The account is already an authorized account");

  authorized_accounts.push_back(account_to_add);

  collections.modify(collection_itr, same_payer, [&](auto& _collection) {
    _collection.authorized_accounts = authorized_accounts;
  });
}


/**
*  Removes an account from the authorized_accounts list of a collection
*  @required_auth The collection author
*/
ACTION atomicassets::remcolauth(
  name collection_name,
  name account_to_remove
) {
  auto collection_itr = collections.require_find(collection_name.value,
  "No collection with this name exists");
  
  require_auth(collection_itr->author);
  vector<name> authorized_accounts = collection_itr->authorized_accounts;

  auto account_itr = std::find(authorized_accounts.begin(), authorized_accounts.end(), account_to_remove);
  
  check(account_itr != authorized_accounts.end(),
  "The account is not an authorized account");
  authorized_accounts.erase(account_itr);

  collections.modify(collection_itr, same_payer, [&](auto& _collection) {
    _collection.authorized_accounts = authorized_accounts;
  });
}


/**
*  Adds an account to the notify_accounts list of a collection
*  This will make the account get notified on every relevant action concerning this collection using require_recipient()
*  NOTE: It will consequently allow the account to make any of these actions throw (fail).
*        Only add trusted accounts to this list
*  @required_atuh The collection author
*/
ACTION atomicassets::addnotifyacc(
  name collection_name,
  name account_to_add
) {
  auto collection_itr = collections.require_find(collection_name.value,
  "No collection with this name exists");
  
  require_auth(collection_itr->author);

  check(is_account(account_to_add), "The account does not exist");

  vector<name> notify_accounts = collection_itr->notify_accounts;

  check(std::find(notify_accounts.begin(), notify_accounts.end(), account_to_add) == notify_accounts.end(),
  "The account is already a nofity account");

  notify_accounts.push_back(account_to_add);

  collections.modify(collection_itr, same_payer, [&](auto& _collection) {
    _collection.notify_accounts = notify_accounts;
  });
}


/**
*  Removes an account from the notify_accounts list of a collection
*  @required_auth The collection author
*/
ACTION atomicassets::remnotifyacc(
  name collection_name,
  name account_to_remove
) {
  auto collection_itr = collections.require_find(collection_name.value,
  "No collection with this name exists");
  
  require_auth(collection_itr->author);
  vector<name> notify_accounts = collection_itr->notify_accounts;

  auto account_itr = std::find(notify_accounts.begin(), notify_accounts.end(), account_to_remove);
  
  check(account_itr != notify_accounts.end(),
  "The account is not a notify account");
  notify_accounts.erase(account_itr);

  collections.modify(collection_itr, same_payer, [&](auto& _collection) {
    _collection.notify_accounts = notify_accounts;
  });
}



/**
*  Creates a new preset
*  @required_auth At least one of the accounts within the authorized_accounts list of the collection
*/
ACTION atomicassets::createpre(
  name authorized_creator,
  name scheme_name,
  name collection_name,
  bool transferable,
  bool burnable,
  uint64_t max_supply,
  ATTRIBUTE_MAP immutable_data,
  ATTRIBUTE_MAP mutable_data
) {
  require_auth(authorized_creator);

  auto scheme_itr = schemes.require_find(scheme_name.value,
  "No scheme with this name exists");
  auto collection_itr = collections.require_find(collection_name.value,
  "No collection with this name exists");

  check(std::find(
    collection_itr->authorized_accounts.begin(),
    collection_itr->authorized_accounts.end(),
    authorized_creator
  ) != collection_itr->authorized_accounts.end(),
  "The creator is not authorized within the collection");

  uint32_t preset_id = presets.available_primary_key();
  presets.emplace(authorized_creator, [&](auto& _preset) {
    _preset.id = preset_id;
    _preset.scheme_name = scheme_name;
    _preset.collection_name = collection_name;
    _preset.transferable = transferable;
    _preset.burnable = burnable;
    _preset.max_supply = max_supply == 0 ? *(new vector<uint8_t>) : int_to_byte_vector(max_supply);
    _preset.issued_supply = max_supply == 0 ? *(new vector<uint8_t>) : int_to_byte_vector(0);
    _preset.immutable_serialized_data = serialize(immutable_data, scheme_itr->format);
    _preset.mutable_serialized_data = serialize(mutable_data, scheme_itr->format);
  });

  action(
    permission_level{get_self(), name("active")},
    get_self(),
    name("lognewpreset"),
    make_tuple(
      preset_id,
      authorized_creator,
      scheme_name,
      collection_name,
      transferable,
      burnable,
      max_supply,
      immutable_data,
      mutable_data
    )
  ).send();
}


/**
*  Updates the mutable data of a preset
*  @required_auth At least one of the accounts within the authorized_accounts list of the collection
*/
ACTION atomicassets::setpredata(
  name authorized_editor,
  uint32_t preset_id,
  ATTRIBUTE_MAP new_mutable_data
) {
  require_auth(authorized_editor);

  auto preset_itr = presets.require_find((uint64_t) preset_id,
  "No preset with this id exists");

  auto collection_itr = collections.find(preset_itr->collection_name.value);
  check(std::find(
    collection_itr->authorized_accounts.begin(),
    collection_itr->authorized_accounts.end(),
    authorized_editor
  ) != collection_itr->authorized_accounts.end(),
  "The editor is not authorized within the collection");

  auto scheme_itr = schemes.find(preset_itr->scheme_name.value);

  presets.modify(preset_itr, authorized_editor, [&](auto& _preset) {
    _preset.mutable_serialized_data = serialize(new_mutable_data, scheme_itr->format);
  });

  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}



/**
*  Creates a new asset
*  Doesn't work if the preset has a specified max_supply that has already been reached
*  @required_auth At least one of the accounts within the authorized_accounts list of the collection
                  specified in the related preset
*/
ACTION atomicassets::mintasset(
  name authorized_minter,
  uint32_t preset_id,
  name new_owner,
  ATTRIBUTE_MAP immutable_data,
  ATTRIBUTE_MAP mutable_data
) {
  require_auth(authorized_minter);

  auto preset_itr = presets.require_find(preset_id,
  "No preset with this id exists");

  auto collection_itr = collections.find(preset_itr->collection_name.value);
  check(std::find(
    collection_itr->authorized_accounts.begin(),
    collection_itr->authorized_accounts.end(),
    authorized_minter
  ) != collection_itr->authorized_accounts.end(),
  "The minter is not authorized within the collection");

  if (preset_itr->max_supply.begin() != preset_itr->max_supply.end()) {
    uint64_t max_supply = byte_vector_to_int(preset_itr->max_supply);
    uint64_t issued_supply = byte_vector_to_int(preset_itr->issued_supply);
    check (issued_supply < max_supply, "preset maxsupply has already been reached");
    presets.modify(preset_itr, authorized_minter, [&](auto& _preset) {
      _preset.issued_supply = int_to_byte_vector(issued_supply + 1);
    });
  }

  auto scheme_itr = schemes.find(preset_itr->scheme_name.value);
  auto current_config = config.get();
  uint64_t asset_id = current_config.asset_counter++;

  assets_t new_owner_assets = get_assets(new_owner);
  new_owner_assets.emplace(authorized_minter, [&](auto& _asset) {
    _asset.id = asset_id;
    _asset.preset_id = preset_id;
    _asset.ram_payer = authorized_minter;
    _asset.backed_core_amount = 0;
    _asset.immutable_serialized_data = serialize(immutable_data, scheme_itr->format);
    _asset.mutable_serialized_data = serialize(mutable_data, scheme_itr->format);
  });
  
  config.set(current_config, get_self());

  action(
    permission_level{get_self(), name("active")},
    get_self(),
    name("logmint"),
    make_tuple(authorized_minter, asset_id, preset_id, new_owner)
  ).send();
}


/**
*  Updates the mutable data of an asset
*  @required_auth At least one of the accounts within the authorized_accounts list of the collection
                  specified in the related preset
*/
ACTION atomicassets::setassetdata(
  name authorized_editor,
  name owner,
  uint64_t asset_id,
  ATTRIBUTE_MAP new_mutable_data
) {
  require_auth(authorized_editor);

  assets_t owner_assets = get_assets(owner);

  auto asset_itr = owner_assets.require_find(asset_id,
  "No asset with this id exists");

  auto preset_itr = presets.find(asset_itr->preset_id);

  auto collection_itr = collections.find(preset_itr->collection_name.value);
  check(std::find(
    collection_itr->authorized_accounts.begin(),
    collection_itr->authorized_accounts.end(),
    authorized_editor
  ) != collection_itr->authorized_accounts.end(),
  "The editor is not authorized within the collection");

  auto scheme_itr = schemes.find(preset_itr->scheme_name.value);

  owner_assets.modify(asset_itr, authorized_editor, [&](auto& _asset) {
    _asset.ram_payer = authorized_editor;
    _asset.mutable_serialized_data = serialize(new_mutable_data, scheme_itr->format);
  });

  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}


/**
*  Burns (deletes) an asset
*  Only works if the "burnable" bool in the related preset is true
*  If the asset has been backed with tokens previously, they are sent to the owner of the asset
*  @required_auth The asset's owner
*/
ACTION atomicassets::burnasset(
  name owner,
  uint64_t asset_id
) {
  require_auth(owner);

  assets_t owner_assets = get_assets(owner);
  auto asset_itr = owner_assets.require_find(asset_id,
  "No asset with this id exists");

  auto preset_itr = presets.find(asset_itr->preset_id);
  check (preset_itr->burnable,
  "The asset is not burnable");


  asset backed_quantity = asset(asset_itr->backed_core_amount, CORE_SYMBOL);

  if (asset_itr->backed_core_amount != 0) {
    action(
      permission_level{get_self(), name("active")},
      name("eosio.token"),
      name("transfer"),
      make_tuple(
        get_self(),
        owner,
        backed_quantity,
        string("Backed asset payout - ID: ") + to_string(asset_id)
      )
    ).send();
  }

  owner_assets.erase(asset_itr);


  auto collection_itr = collections.find(preset_itr->collection_name.value);
  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}


/**
*  Creates an offer
*  Offers are two sided, with the only requirement being that at least one asset is included in one of the sides
*/
ACTION atomicassets::createoffer(
  name sender,
  name recipient,
  vector<uint64_t> sender_asset_ids,
  vector<uint64_t> recipient_asset_ids,
  string memo
) {
  require_auth(sender);

  require_recipient(sender);
  require_recipient(recipient);

  check(sender != recipient, "Can't send an offer to yourself");

  check(sender_asset_ids.size() != 0 || recipient_asset_ids.size() != 0,
  "Can't create an empty offer"); 

  assets_t sender_assets = get_assets(sender);
  assets_t recipient_assets = get_assets(recipient);

  for (uint64_t asset_id : sender_asset_ids) {
    auto asset_itr = sender_assets.require_find(asset_id,
    ("Offer sender doesn't own at least one of the provided assets (ID: " + to_string(asset_id) + ")").c_str());
    auto preset_itr = presets.find(asset_itr->preset_id);
    check(preset_itr->transferable,
    ("At least one asset isn't transferable (ID: " + to_string(asset_id) + ")").c_str());
  }
  for (uint64_t asset_id : recipient_asset_ids) {
    auto asset_itr = recipient_assets.require_find(asset_id,
    ("Offer recipient doesn't own at least one of the provided assets (ID: " + to_string(asset_id) + ")").c_str());
    auto preset_itr = presets.find(asset_itr->preset_id);
    check(preset_itr->transferable,
    ("At least one asset isn't transferable (ID: " + to_string(asset_id) + ")").c_str());
  }

  config_s current_config = config.get();
  offers.emplace(sender, [&](auto& _offer) {
    _offer.id = current_config.offer_counter++;
    _offer.offer_sender = sender;
    _offer.offer_recipient = recipient;
    _offer.sender_asset_ids = sender_asset_ids;
    _offer.recipient_asset_ids = recipient_asset_ids;
    _offer.memo = memo;
  });

  config.set(current_config, get_self());
}


/**
*  Cancels (deletes) an existing offer
*  @required_auth The offer's creator
*/
ACTION atomicassets::canceloffer(
  uint64_t offer_id
) {
  auto offer_itr = offers.require_find(offer_id,
  "No offer with this id exists");

  require_auth(offer_itr->offer_sender);

  offers.erase(offer_itr);
}


/**
*  Accepts an offer
*  The items specified in the offer from either side are transferred to the corresponding other side
*  If a new scope needs to be created, each side of the offer will pay for their own scope
*  @require_auth The offer's recipient
*/
ACTION atomicassets::acceptoffer(
  uint64_t offer_id
) {
  auto offer_itr = offers.require_find(offer_id,
  "No offer with this id exists");

  require_auth(offer_itr->offer_recipient);

  require_recipient(offer_itr->offer_sender);
  require_recipient(offer_itr->offer_recipient);

  assets_t sender_assets = get_assets(offer_itr->offer_sender);
  assets_t recipient_assets = get_assets(offer_itr->offer_recipient);
  for (uint64_t asset_id : offer_itr->sender_asset_ids) {
    sender_assets.require_find(asset_id,
    ("Offer sender doesn't own at least one of the provided assets (ID: " + to_string(asset_id) + ")").c_str());
  }
  for (uint64_t asset_id : offer_itr->recipient_asset_ids) {
    recipient_assets.require_find(asset_id,
    ("Offer recipient doesn't own at least one of the provided assets (ID: " + to_string(asset_id) + ")").c_str());
  }

  if (offer_itr->recipient_asset_ids.size() != 0) {
    //Potential scope costs for offer sender are offset by removing the entry from the offer table
    internal_transfer(
      offer_itr->offer_recipient,
      offer_itr->offer_sender,
      offer_itr->recipient_asset_ids,
      string("Accepted Offer ID: " + to_string(offer_id)),
      offer_itr->offer_sender
    );
  }

  if (offer_itr->sender_asset_ids.size() != 0) {
    internal_transfer(
      offer_itr->offer_sender,
      offer_itr->offer_recipient,
      offer_itr->sender_asset_ids,
      string("Accepted Offer ID: " + to_string(offer_id)),
      offer_itr->offer_recipient
    );
  }

  offers.erase(offer_itr);
}


ACTION atomicassets::declineoffer(
  uint64_t offer_id
) {
  auto offer_itr = offers.require_find(offer_id,
  "No offer with this id exists");

  require_auth(offer_itr->offer_recipient);

  offers.erase(offer_itr);
}




/**
*  This function is called when a transfer receipt from eosio.token is sent to the contract
*  It handles potential back_asset transfers, which back an asset with tokens which can only be
   released by burning the token, thus giving the token a guranteed value
*/
void atomicassets::receive_token_transfer(name from, name to, asset quantity, string memo) {
  if (to != _self) {
    return;
  }
  check(quantity.symbol == CORE_SYMBOL,
  "quantity must be the Core symbol");

  if (memo.find("back_asset ") == 0) {
    //Format: "back_asset <account name> <assetid>"
    string working_str = memo.substr(11);
    string account_str = working_str.substr(0, working_str.find(' '));
    string id_str = working_str.substr(working_str.find(' ') + 1);

    name account = name(account_str);
    uint64_t asset_id = stoll(id_str);

    assets_t account_assets = get_assets(account);
    auto asset_itr = account_assets.require_find(asset_id,
    "The account does not own an asset with this id");

    auto preset_itr = presets.find(asset_itr->preset_id);
    check(preset_itr->burnable, "Can't back an asset that is not burnable");

    account_assets.modify(asset_itr, same_payer, [&](auto& _asset) {
      _asset.backed_core_amount += quantity.amount;
    });

    action(
      permission_level{get_self(), name("active")},
      get_self(),
      name("logbackasset"),
      make_tuple(account, asset_id, quantity)
    ).send();


  } else {
    check(false, "invalid memo");
  }
}



ACTION atomicassets::logtransfer(
  name collection_name,
  name from,
  name to,
  vector<uint64_t> asset_ids,
  string memo,
  name scope_payer
) {
  require_auth(get_self());

  auto collection_itr = collections.find(collection_name.value);
  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}


ACTION atomicassets::lognewpreset(
  uint32_t preset_id,
  name authorized_creator,
  name scheme_name,
  name collection_name,
  bool transferable,
  bool burnable,
  uint64_t max_supply,
  ATTRIBUTE_MAP immutable_data,
  ATTRIBUTE_MAP mutable_data
) {
  require_auth(get_self());

  auto collection_itr = collections.find(collection_name.value);
  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}


ACTION atomicassets::logmint(
  name minter,
  uint64_t asset_id,
  uint32_t preset_id,
  name new_owner
) {
  require_auth(get_self());

  auto preset_itr = presets.find(preset_id);
  auto collection_itr = collections.find(preset_itr->collection_name.value);
  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}


ACTION atomicassets::logbackasset(
  name owner,
  uint64_t asset_id,
  asset back_quantity
) {
  require_auth(get_self());

  assets_t owner_assets = get_assets(owner);
  auto asset_itr = owner_assets.find(asset_id);
  auto preset_itr = presets.find(asset_itr->preset_id);
  auto collection_itr = collections.find(preset_itr->collection_name.value);
  for (const name& notify_account : collection_itr->notify_accounts) {
    require_recipient(notify_account);
  }
}



/**
*  Transfers need to be handled like this (as a function instead of an action), because when accepting an offer,
*  we want each side of the offer to pay for their own scope. Because the recipient authorized the accept action,
*  he can be charged the RAM for his own scope, and because the offer is removed from the table, which was previously
*  paid by the offer sender, the action RAM delta for the sender account will still be positive even after paying
*  for the scope. This is allowed by the protocol feature RAM_RESTRICTIONS which needs to be enabled on the blockchain
*  that this contract is deployed on.
*/
void atomicassets::internal_transfer(
  name from,
  name to,
  vector<uint64_t> asset_ids,
  string memo,
  name scope_payer
) {
  check(is_account(to), "to account does not exist");

  require_recipient(from);
  require_recipient(to);

  check(from != to, "Can't transfer assets to yourself");

  check(asset_ids.size() != 0, "asset_ids needs to contain at least one id");

  assets_t from_assets = get_assets(from);
  assets_t to_assets = get_assets(to);

  map<name, vector<uint64_t>> collection_to_assets_transferred = {};

  for (uint64_t asset_id : asset_ids) {
    auto asset_itr = from_assets.require_find(asset_id,
    ("Sender doesn't own at least one of the provided assets (ID: " + to_string(asset_id) + ")").c_str());

    //Existence doesn't have to be checked because this always has to exist
    auto preset_itr = presets.find(asset_itr->preset_id);
    check(preset_itr->transferable,
    ("At least one asset isn't transferable (ID: " + to_string(asset_id) + ")").c_str());

    //This is needed for sending noficiations later
    if (collection_to_assets_transferred.find(preset_itr->collection_name) != collection_to_assets_transferred.end()) {
      collection_to_assets_transferred[preset_itr->collection_name].push_back(asset_id);
    } else {
      collection_to_assets_transferred[preset_itr->collection_name] = {asset_id};
    }

    //to assets are empty => no scope has been created yet
    bool no_previous_scope = to_assets.begin() == to_assets.end();
    if (no_previous_scope) {
      //A dummy asset is emplaced, which makes the scope_payer pay for the ram of the scope
      //This asset is later deleted again.
      //This action will therefore fail is the scope_payer didn't authorize the action
      to_assets.emplace(scope_payer, [&](auto& _asset) {
        _asset.id = MAX_ID;
        _asset.preset_id = 0;
        _asset.ram_payer = name("");
        _asset.backed_core_amount = 0;
        _asset.immutable_serialized_data = {};
        _asset.mutable_serialized_data = {};
      });
    }

    to_assets.emplace(asset_itr->ram_payer, [&](auto& _asset) {
      _asset.id = asset_itr->id;
      _asset.preset_id = asset_itr->preset_id;
      _asset.ram_payer = asset_itr->ram_payer;
      _asset.backed_core_amount = asset_itr->backed_core_amount;
      _asset.immutable_serialized_data = asset_itr->immutable_serialized_data;
      _asset.mutable_serialized_data = asset_itr->mutable_serialized_data;
    });

    from_assets.erase(asset_itr);

    if (no_previous_scope) {
      to_assets.erase(to_assets.find(MAX_ID));
    }
  }

  //Sending notifications
  for (const auto& [collection, assets_transferred] : collection_to_assets_transferred) {
    action(
      permission_level{get_self(), name("active")},
      get_self(),
      name("logtransfer"),
      make_tuple(collection, from, to, assets_transferred, memo, scope_payer)
    ).send();
  }
}





atomicassets::assets_t atomicassets::get_assets(name acc) {
  return assets_t(get_self(), acc.value);
}


vector<uint8_t> atomicassets::int_to_byte_vector(uint64_t number) {
  vector<uint8_t> bytes = {};
  while (number != 0) {
    bytes.push_back((uint8_t) number % 256);
    number /= 256;
  }
  return bytes;
}


uint64_t atomicassets::byte_vector_to_int(vector<uint8_t> bytes) {
  uint64_t number = 0;
  uint64_t multiplier = 1;
  for (const auto& byte_val : bytes) {
    number += ((uint64_t) byte_val) * multiplier;
    multiplier *= 256;
  }
  return number;
}
