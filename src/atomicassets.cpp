#include <atomicassets.hpp>


/**
*  Initializes the config table. Only needs to be called once when first deploying the contract
*  @required_auth The contract itself
*/
ACTION atomicassets::init() {
    require_auth(get_self());
    config.get_or_create(get_self(), config_s{});
    tokenconfigs.get_or_create(get_self(), tokenconfigs_s{});
}

/**
*  Adds one or more lines to the format that is used for collection data serialization
*  @required_auth The contract itself
*/
ACTION atomicassets::admincoledit(vector <atomicdata::FORMAT> collection_format_extension) {
    require_auth(get_self());

    check(collection_format_extension.size() != 0, "Need to add at least one new line");

    config_s current_config = config.get();
    current_config.collection_format.insert(
        current_config.collection_format.end(),
        collection_format_extension.begin(),
        collection_format_extension.end()
    );
    check_format(current_config.collection_format);

    config.set(current_config, get_self());
}


/**
*  Sets the version for the tokenconfigs table
*  @required_auth The contract itself
*/
ACTION atomicassets::setversion(string new_version) {
    require_auth(get_self());

    tokenconfigs_s current_tokenconfigs = tokenconfigs.get();
    current_tokenconfigs.version = new_version;

    tokenconfigs.set(current_tokenconfigs, get_self());
}


/**
*  Adds a token that can then be backed to assets
*  @required_auth The contract itself
*/
ACTION atomicassets::addconftoken(name token_contract, symbol token_symbol) {
    require_auth(get_self());

    config_s current_config = config.get();
    for (extended_symbol token : current_config.supported_tokens) {
        check(token.get_symbol() != token_symbol,
            "A token with this symbol is already supported");
    }

    current_config.supported_tokens.push_back(extended_symbol(token_symbol, token_contract));

    config.set(current_config, get_self());
}


/**
*  Transfers one or more assets to another account
*  @required_auth The from account
*/
ACTION atomicassets::transfer(
    name from,
    name to,
    vector <uint64_t> asset_ids,
    string memo
) {
    require_auth(from);
    require_recipient(from);
    require_recipient(to);
    internal_transfer(from, to, asset_ids, memo, from);
}


/**
*  Creates a new collection
*/
ACTION atomicassets::createcol(
    name author,
    name collection_name,
    bool allow_notify,
    vector <name> authorized_accounts,
    vector <name> notify_accounts,
    double market_fee,
    ATTRIBUTE_MAP data
) {
    require_auth(author);

    name collection_name_suffix = collection_name.suffix();

    if (is_account(collection_name)) {
        check(has_auth(collection_name),
            "When the collection has the name of an existing account, its authorization is required");
    } else {
        if (collection_name_suffix != collection_name) {
            check(has_auth(collection_name_suffix),
                "When the collection name has a suffix, the suffix authorization is required");
        } else {
            check(collection_name.length() == 12,
                "Without special authorization, collection names must be 12 characters long");
        }
    }

    check(collections.find(collection_name.value) == collections.end(),
        "A collection with this name already exists");

    check(allow_notify || notify_accounts.size() == 0, "Can't add notify_accounts if allow_notify is false");

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

    check(0 <= market_fee && market_fee <= MAX_MARKET_FEE,
        "The market_fee must be between 0 and " + to_string(MAX_MARKET_FEE));

    check_name_length(data);

    config_s current_config = config.get();

    collections.emplace(author, [&](auto &_collection) {
        _collection.collection_name = collection_name;
        _collection.author = author;
        _collection.allow_notify = allow_notify;
        _collection.authorized_accounts = authorized_accounts;
        _collection.notify_accounts = notify_accounts;
        _collection.market_fee = market_fee;
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

    check_name_length(data);

    config_s current_config = config.get();
    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
        _collection.serialized_data = serialize(data, current_config.collection_format);
    });
}


/**
*  Adds an account to the authorized_accounts list of a collection
*  This will allow the account to create and edit both templates and assets that belong to this collection
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

    vector <name> authorized_accounts = collection_itr->authorized_accounts;
    check(std::find(authorized_accounts.begin(), authorized_accounts.end(), account_to_add) ==
          authorized_accounts.end(),
        "The account is already an authorized account");

    authorized_accounts.push_back(account_to_add);

    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
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
    vector <name> authorized_accounts = collection_itr->authorized_accounts;

    auto account_itr = std::find(authorized_accounts.begin(), authorized_accounts.end(), account_to_remove);

    check(account_itr != authorized_accounts.end(),
        "The account is not an authorized account");
    authorized_accounts.erase(account_itr);

    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
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

    check(collection_itr->allow_notify, "Adding notify accounts to this collection is not allowed");

    check(is_account(account_to_add), "The account does not exist");

    vector <name> notify_accounts = collection_itr->notify_accounts;

    check(std::find(notify_accounts.begin(), notify_accounts.end(), account_to_add) == notify_accounts.end(),
        "The account is already a notify account");

    notify_accounts.push_back(account_to_add);

    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
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
    vector <name> notify_accounts = collection_itr->notify_accounts;

    auto account_itr = std::find(notify_accounts.begin(), notify_accounts.end(), account_to_remove);

    check(account_itr != notify_accounts.end(),
        "The account is not a notify account");
    notify_accounts.erase(account_itr);

    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
        _collection.notify_accounts = notify_accounts;
    });
}


/**
* Sets (changes) the market fee for an existing collection
* @required_auth The collection author
*/
ACTION atomicassets::setmarketfee(
    name collection_name,
    double market_fee
) {
    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    require_auth(collection_itr->author);

    check(0 <= market_fee && market_fee <= MAX_MARKET_FEE,
        "The market_fee must be between 0 and " + to_string(MAX_MARKET_FEE));

    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
        _collection.market_fee = market_fee;
    });
}


/**
* Sets allow_notify to false for a collection where it has previously been true
* The collection's notify_accounts list must be empty
* @required_auth The collection author
*/
ACTION atomicassets::forbidnotify(
    name collection_name
) {
    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    require_auth(collection_itr->author);

    check(collection_itr->notify_accounts.size() == 0, "The collection's notify_accounts vector must be empty");

    check(collection_itr->allow_notify, "allow_notify is already false for this collection");

    collections.modify(collection_itr, same_payer, [&](auto &_collection) {
        _collection.allow_notify = false;
    });
}


/**
*  Creates a new schema
*  schemas can only be extended in the future, but never changed retroactively.
*  This guarantees a correct deserialization for existing templates and assets.
*  @required_auth authorized_creator, who is within the authorized_accounts list of the collection
*/
ACTION atomicassets::createschema(
    name authorized_creator,
    name collection_name,
    name schema_name,
    vector <FORMAT> schema_format
) {
    require_auth(authorized_creator);

    check(1 <= schema_name.length() && schema_name.length() <= 12,
        "Schema names must be between 1 and 12 characters long");

    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    check_has_collection_auth(
        authorized_creator,
        collection_name,
        "The creator is not authorized within the collection"
    );

    schemas_t collection_schemas = get_schemas(collection_name);

    check(collection_schemas.find(schema_name.value) == collection_schemas.end(),
        "A schema with this name already exists for this collection");

    check_format(schema_format);

    collection_schemas.emplace(authorized_creator, [&](auto &_schema) {
        _schema.schema_name = schema_name;
        _schema.format = schema_format;
    });
}


/**
*  Adds one or more lines to the format of an existing schema
*  @required_auth authorized_editor, who is within the authorized_accounts list of the collection
*/
ACTION atomicassets::extendschema(
    name authorized_editor,
    name collection_name,
    name schema_name,
    vector <FORMAT> schema_format_extension
) {
    require_auth(authorized_editor);

    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    check_has_collection_auth(
        authorized_editor,
        collection_name,
        "The editor is not authorized within the collection"
    );

    check(schema_format_extension.size() != 0, "Need to add at least one new line");

    schemas_t collection_schemas = get_schemas(collection_name);
    auto schema_itr = collection_schemas.require_find(schema_name.value,
        "No schema with this name exists for this collection");

    vector <FORMAT> lines = schema_itr->format;
    lines.insert(lines.end(), schema_format_extension.begin(), schema_format_extension.end());
    check_format(lines);

    collection_schemas.modify(schema_itr, authorized_editor, [&](auto &_schema) {
        _schema.format = lines;
    });
}


/**
*  Creates a new template
*  @required_auth authorized_creator, who is within the authorized_accounts list of the collection
*/
ACTION atomicassets::createtempl(
    name authorized_creator,
    name collection_name,
    name schema_name,
    bool transferable,
    bool burnable,
    uint32_t max_supply,
    ATTRIBUTE_MAP immutable_data
) {
    require_auth(authorized_creator);

    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    check_has_collection_auth(
        authorized_creator,
        collection_name,
        "The creator is not authorized within the collection"
    );

    schemas_t collection_schemas = get_schemas(collection_name);
    auto schema_itr = collection_schemas.require_find(schema_name.value,
        "No schema with this name exists");

    config_s current_config = config.get();
    int32_t template_id = current_config.template_counter++;
    config.set(current_config, get_self());

    templates_t collection_templates = get_templates(collection_name);

    collection_templates.emplace(authorized_creator, [&](auto &_template) {
        _template.template_id = template_id;
        _template.schema_name = schema_name;
        _template.transferable = transferable;
        _template.burnable = burnable;
        _template.max_supply = max_supply;
        _template.issued_supply = 0;
        _template.immutable_serialized_data = serialize(immutable_data, schema_itr->format);
    });

    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("lognewtempl"),
        make_tuple(
            template_id,
            authorized_creator,
            collection_name,
            schema_name,
            transferable,
            burnable,
            max_supply,
            immutable_data
        )
    ).send();
}


/**
* Sets the max supply of the template to the issued supply
* This means that afterwards no new assets of this template can be minted
* @required_auth authorized_editor, who is within the authorized_accounts list of the collection
**/
ACTION atomicassets::locktemplate(
    name authorized_editor,
    name collection_name,
    int32_t template_id
) {
    require_auth(authorized_editor);

    check(template_id >= 0, "The template id must be positive");

    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    check_has_collection_auth(
        authorized_editor,
        collection_name,
        "The editor is not authorized within the collection"
    );

    templates_t collection_templates = get_templates(collection_name);
    auto template_itr = collection_templates.require_find(template_id,
        "No template with the specified id exists for the specified collection");

    check(template_itr->issued_supply != 0,
        "Can't lock a template that does not have at least one issued asset");

    collection_templates.modify(template_itr, same_payer, [&](auto &_template) {
        _template.max_supply = _template.issued_supply;
    });
}


/**
*  Creates a new asset
*  Doesn't work if the template has a specified max_supply that has already been reached
*  @required_auth authorized_minter, who is within the authorized_accounts list of the collection
                  specified in the related template
*/
ACTION atomicassets::mintasset(
    name authorized_minter,
    name collection_name,
    name schema_name,
    int32_t template_id,
    name new_asset_owner,
    ATTRIBUTE_MAP immutable_data,
    ATTRIBUTE_MAP mutable_data,
    vector <asset> tokens_to_back
) {
    require_auth(authorized_minter);

    auto collection_itr = collections.find(collection_name.value);

    check_has_collection_auth(
        authorized_minter,
        collection_name,
        "The minter is not authorized within the collection"
    );

    schemas_t collection_schemas = get_schemas(collection_name);
    auto schema_itr = collection_schemas.require_find(schema_name.value,
        "No schema with this name exists");

    //Needed for the log action
    ATTRIBUTE_MAP deserialized_template_data;
    if (template_id >= 0) {
        templates_t collection_templates = get_templates(collection_name);

        auto template_itr = collection_templates.require_find(template_id,
            "No template with this id exists");

        check(template_itr->schema_name == schema_name,
            "The template belongs to another schema");

        if (template_itr->max_supply > 0) {
            check(template_itr->issued_supply < template_itr->max_supply,
                "The template's maxsupply has already been reached");
        }
        collection_templates.modify(template_itr, same_payer, [&](auto &_template) {
            _template.issued_supply += 1;
        });

        deserialized_template_data = deserialize(
            template_itr->immutable_serialized_data,
            schema_itr->format
        );
    } else {
        check(template_id == -1, "The template id must either be an existing template or -1");

        deserialized_template_data = {};
    }

    check(is_account(new_asset_owner), "The new_asset_owner account does not exist");

    check_name_length(immutable_data);
    check_name_length(mutable_data);

    config_s current_config = config.get();
    uint64_t asset_id = current_config.asset_counter++;
    config.set(current_config, get_self());

    assets_t new_owner_assets = get_assets(new_asset_owner);
    new_owner_assets.emplace(authorized_minter, [&](auto &_asset) {
        _asset.asset_id = asset_id;
        _asset.collection_name = collection_name;
        _asset.schema_name = schema_name;
        _asset.template_id = template_id;
        _asset.ram_payer = authorized_minter;
        _asset.backed_tokens = {};
        _asset.immutable_serialized_data = serialize(immutable_data, schema_itr->format);
        _asset.mutable_serialized_data = serialize(mutable_data, schema_itr->format);
    });


    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("logmint"),
        make_tuple(
            asset_id,
            authorized_minter,
            collection_name,
            schema_name,
            template_id,
            new_asset_owner,
            immutable_data,
            mutable_data,
            tokens_to_back,
            deserialized_template_data
        )
    ).send();

    //Calls the internal_back_asset function which handles asset backing.
    //It will throw if authorized_minter does not have a sufficient balance to pay for the backed tokens
    //Token validity must not be cross-checked with config.supported_tokens because it's implicitly checked
    //when decreasing minter's balance (only supported tokens can be deposited)
    set <symbol> used_symbols = {};
    for (asset &token : tokens_to_back) {
        check(used_symbols.find(token.symbol) == used_symbols.end(),
            "Symbols in the tokens_to_back must be unique");
        used_symbols.emplace(token.symbol);
        internal_back_asset(authorized_minter, new_asset_owner, asset_id, token);
    }
}


/**
*  Updates the mutable data of an asset
*  @required_auth authorized_editor, who is within the authorized_accounts list of the collection
                  specified in the related template
*/
ACTION atomicassets::setassetdata(
    name authorized_editor,
    name asset_owner,
    uint64_t asset_id,
    ATTRIBUTE_MAP new_mutable_data
) {
    require_auth(authorized_editor);

    assets_t owner_assets = get_assets(asset_owner);

    auto asset_itr = owner_assets.require_find(asset_id,
        "No asset with this id exists");

    auto collection_itr = collections.find(asset_itr->collection_name.value);

    check_has_collection_auth(
        authorized_editor,
        asset_itr->collection_name,
        "The editor is not authorized within the collection"
    );

    check_name_length(new_mutable_data);

    schemas_t collection_schemas = get_schemas(asset_itr->collection_name);
    auto schema_itr = collection_schemas.find(asset_itr->schema_name.value);

    ATTRIBUTE_MAP deserialized_old_data = deserialize(
        asset_itr->mutable_serialized_data,
        schema_itr->format
    );

    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("logsetdata"),
        make_tuple(asset_owner, asset_id, deserialized_old_data, new_mutable_data)
    ).send();


    owner_assets.modify(asset_itr, authorized_editor, [&](auto &_asset) {
        _asset.ram_payer = authorized_editor;
        _asset.mutable_serialized_data = serialize(new_mutable_data, schema_itr->format);
    });
}


/**
* This action is used to add a zero value asset to the quantities vector of owner in the balances table
* If no row exists for owner, a new one is created
* This action needs to be called before transferring (depositing) any tokens to the AtomicAssets smart contract,
* in order to pay for the RAM that otherwise would have to be paid by the AtomicAssets smart contract
*
* To pass a symbol to eosio as a string, use the following format: <precision>,<symbol_code>
* So for example: "8,WAX"
*
* @required_auth owner
*/
ACTION atomicassets::announcedepo(
    name owner,
    symbol symbol_to_announce
) {
    require_auth(owner);

    config_s current_config = config.get();

    bool is_supported = false;
    for (extended_symbol supported_token : current_config.supported_tokens) {
        if (supported_token.get_symbol() == symbol_to_announce) {
            is_supported = true;
            break;
        }
    }
    check(is_supported, "The specified symbol is not supported");

    auto balance_itr = balances.find(owner.value);

    if (balance_itr == balances.end()) {
        vector <asset> quantities = {asset(0, symbol_to_announce)};
        balances.emplace(owner, [&](auto &_balance) {
            _balance.owner = owner;
            _balance.quantities = quantities;
        });

    } else {
        vector <asset> quantities = balance_itr->quantities;
        for (asset &token : quantities) {
            if (token.symbol == symbol_to_announce) {
                //The symbol has already been announced
                return;
            }
        }
        quantities.push_back(asset(0, symbol_to_announce));

        balances.modify(balance_itr, owner, [&](auto &_balance) {
            _balance.quantities = quantities;
        });
    }
}


/**
* Withdraws fungible tokens that were previously deposited
*
* @required_auth owner
*/
ACTION atomicassets::withdraw(
    name owner,
    asset token_to_withdraw
) {
    require_auth(owner);

    check(token_to_withdraw.amount > 0, "token_to_withdraw must be positive");

    //The internal_decrease_balance function will throw if owner does not have a sufficient balance
    internal_decrease_balance(owner, token_to_withdraw);

    config_s current_config = config.get();

    for (extended_symbol supported_token : current_config.supported_tokens) {
        if (supported_token.get_symbol() == token_to_withdraw.symbol) {
            action(
                permission_level{get_self(), name("active")},
                supported_token.get_contract(),
                name("transfer"),
                make_tuple(
                    get_self(),
                    owner,
                    token_to_withdraw,
                    string("Withdrawal")
                )
            ).send();
            break;
        }
    }
}


/**
* Backs an asset with a fungible token that was previously deposited by payer
* payer also pays for the full RAM of the asset that is backed
* 
* @required_auth payer
*/
ACTION atomicassets::backasset(
    name payer,
    name asset_owner,
    uint64_t asset_id,
    asset token_to_back
) {
    require_auth(payer);

    internal_back_asset(payer, asset_owner, asset_id, token_to_back);
}


/**
*  Burns (deletes) an asset
*  Only works if the "burnable" bool in the related template is true
*  If the asset has been backed with tokens previously, they are sent to the owner of the asset
*  @required_auth asset_owner
*/
ACTION atomicassets::burnasset(
    name asset_owner,
    uint64_t asset_id
) {
    require_auth(asset_owner);

    assets_t owner_assets = get_assets(asset_owner);
    auto asset_itr = owner_assets.require_find(asset_id,
        "No asset with this id exists for this owner");

    if (asset_itr->template_id >= 0) {
        templates_t collection_templates = get_templates(asset_itr->collection_name);

        auto template_itr = collection_templates.find(asset_itr->template_id);
        check(template_itr->burnable, "The asset is not burnable");
    };


    if (asset_itr->backed_tokens.size() != 0) {
        auto balance_itr = balances.find(asset_owner.value);
        if (balance_itr == balances.end()) {
            // If the asset_owner does not have a balance table entry yet, a new one is created
            balances.emplace(asset_owner, [&](auto &_balance) {
                _balance.owner = asset_owner,
                _balance.quantities = asset_itr->backed_tokens;
            });
        } else {
            // Any backed tokens are added to the asset_owners balance
            vector <asset> quantities = balance_itr->quantities;

            for (asset backed_quantity : asset_itr->backed_tokens) {
                bool found_token = false;
                for (asset &token : quantities) {
                    if (token.symbol == backed_quantity.symbol) {
                        found_token = true;
                        token.amount += backed_quantity.amount;
                        break;
                    }
                }
                if (!found_token) {
                    quantities.push_back(backed_quantity);
                }
            }

            balances.modify(balance_itr, asset_owner, [&](auto &_balance) {
                _balance.quantities = quantities;
            });
        }
    }


    schemas_t collection_schemas = get_schemas(asset_itr->collection_name);
    auto schema_itr = collection_schemas.find(asset_itr->schema_name.value);

    ATTRIBUTE_MAP deserialized_immutable_data = deserialize(
        asset_itr->immutable_serialized_data,
        schema_itr->format
    );
    ATTRIBUTE_MAP deserialized_mutable_data = deserialize(
        asset_itr->mutable_serialized_data,
        schema_itr->format
    );

    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("logburnasset"),
        make_tuple(
            asset_owner,
            asset_id,
            asset_itr->collection_name,
            asset_itr->schema_name,
            asset_itr->template_id,
            asset_itr->backed_tokens,
            deserialized_immutable_data,
            deserialized_mutable_data,
            asset_itr->ram_payer
        )
    ).send();

    owner_assets.erase(asset_itr);
}


/**
*  Creates an offer
*  Offers are two sided, with the only requirement being that at least one asset is included in one of the sides
*  @required_auth sender
*/
ACTION atomicassets::createoffer(
    name sender,
    name recipient,
    vector <uint64_t> sender_asset_ids,
    vector <uint64_t> recipient_asset_ids,
    string memo
) {
    require_auth(sender);

    check(is_account(recipient), "The recipient account deos not exist");

    check(sender != recipient, "Can't send an offer to yourself");

    check(sender_asset_ids.size() != 0 || recipient_asset_ids.size() != 0,
        "Can't create an empty offer");

    check(memo.length() <= 256, "An offer memo can only be 256 characters max");

    vector <uint64_t> sender_ids_copy = sender_asset_ids;
    std::sort(sender_ids_copy.begin(), sender_ids_copy.end());
    check(std::adjacent_find(sender_ids_copy.begin(), sender_ids_copy.end()) == sender_ids_copy.end(),
        "The assets in sender_asset_ids must be unique");

    vector <uint64_t> recipient_ids_copy = recipient_asset_ids;
    std::sort(recipient_ids_copy.begin(), recipient_ids_copy.end());
    check(std::adjacent_find(recipient_ids_copy.begin(), recipient_ids_copy.end()) == recipient_ids_copy.end(),
        "The assets in recipient_asset_ids must be unique");

    assets_t sender_assets = get_assets(sender);
    assets_t recipient_assets = get_assets(recipient);

    for (uint64_t asset_id : sender_asset_ids) {
        auto asset_itr = sender_assets.require_find(asset_id,
            ("Offer sender doesn't own at least one of the provided assets (ID: " +
             to_string(asset_id) + ")").c_str());
        if (asset_itr->template_id >= 0) {
            templates_t collection_templates = get_templates(asset_itr->collection_name);

            auto template_itr = collection_templates.find(asset_itr->template_id);
            check(template_itr->transferable,
                ("At least one asset isn't transferable (ID: " + to_string(asset_id) + ")").c_str());
        }
    }
    for (uint64_t asset_id : recipient_asset_ids) {
        auto asset_itr = recipient_assets.require_find(asset_id,
            ("Offer recipient doesn't own at least one of the provided assets (ID: " +
             to_string(asset_id) + ")").c_str());
        if (asset_itr->template_id >= 0) {
            templates_t collection_templates = get_templates(asset_itr->collection_name);

            auto template_itr = collection_templates.find(asset_itr->template_id);
            check(template_itr->transferable,
                ("At least one asset isn't transferable (ID: " + to_string(asset_id) + ")").c_str());
        }
    }

    config_s current_config = config.get();
    uint64_t offer_id = current_config.offer_counter++;
    offers.emplace(sender, [&](auto &_offer) {
        _offer.offer_id = offer_id;
        _offer.sender = sender;
        _offer.recipient = recipient;
        _offer.sender_asset_ids = sender_asset_ids;
        _offer.recipient_asset_ids = recipient_asset_ids;
        _offer.memo = memo;
        _offer.ram_payer = sender;
    });

    config.set(current_config, get_self());

    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("lognewoffer"),
        make_tuple(offer_id, sender, recipient, sender_asset_ids, recipient_asset_ids, memo)
    ).send();
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

    require_auth(offer_itr->sender);

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

    require_auth(offer_itr->recipient);

    require_recipient(offer_itr->sender);
    require_recipient(offer_itr->recipient);

    assets_t sender_assets = get_assets(offer_itr->sender);
    assets_t recipient_assets = get_assets(offer_itr->recipient);
    for (uint64_t asset_id : offer_itr->sender_asset_ids) {
        sender_assets.require_find(asset_id,
            ("Offer sender doesn't own at least one of the provided assets (ID: " +
             to_string(asset_id) + ")").c_str());
    }
    for (uint64_t asset_id : offer_itr->recipient_asset_ids) {
        recipient_assets.require_find(asset_id,
            ("Offer recipient doesn't own at least one of the provided assets (ID: " +
             to_string(asset_id) + ")").c_str());
    }

    if (offer_itr->recipient_asset_ids.size() != 0) {
        //Potential scope costs for offer sender are offset by removing the entry from the offer table
        internal_transfer(
            offer_itr->recipient,
            offer_itr->sender,
            offer_itr->recipient_asset_ids,
            string("Accepted Offer ID: " + to_string(offer_id)),
            offer_itr->ram_payer
        );
    }

    if (offer_itr->sender_asset_ids.size() != 0) {
        internal_transfer(
            offer_itr->sender,
            offer_itr->recipient,
            offer_itr->sender_asset_ids,
            string("Accepted Offer ID: " + to_string(offer_id)),
            offer_itr->recipient
        );
    }

    offers.erase(offer_itr);
}


/**
*  Declines an offer
*  The offer is then erased from the tables
*  @require_auth The offer's recipient
*/
ACTION atomicassets::declineoffer(
    uint64_t offer_id
) {
    auto offer_itr = offers.require_find(offer_id,
        "No offer with this id exists");

    require_auth(offer_itr->recipient);

    offers.erase(offer_itr);
}


/**
* Pays for the RAM of an existing offer (thus freeing the RAM of the previous payer)
* The main purpose for this is to allow dapps to pay for the RAM of offer that their users create
* in order to make sure that the users don't run out of RAM
* @require_auth payer
*/
ACTION atomicassets::payofferram(
    name payer,
    uint64_t offer_id
) {
    require_auth(payer);

    auto offer_itr = offers.require_find(offer_id,
        "No offer with this id exists");

    offers_s offer_copy = *offer_itr;

    offers.erase(offer_itr);

    offers.emplace(payer, [&](auto &_offer) {
        _offer = offer_copy;
        _offer.ram_payer = payer;
    });
}


/**
*  This function is called when a transfer receipt from any token contract is sent to the atomicassets contract
*  It handels deposits and adds the transferred tokens to the sender's balance table row
*/
void atomicassets::receive_token_transfer(name from, name to, asset quantity, string memo) {
    if (to != get_self()) {
        return;
    }

    config_s current_config = config.get();

    bool is_supported = false;
    for (extended_symbol token : current_config.supported_tokens) {
        if (token.get_contract() == get_first_receiver() && token.get_symbol() == quantity.symbol) {
            is_supported = true;
        }
    }
    check(is_supported, "The transferred token is not supported");

    if (memo == "deposit") {
        auto balance_itr = balances.require_find(from.value,
            "You need to first initialize the balance table row using the announcedepo action");

        //Quantities refers to the quantities value in the balances table row, quantity is the asset that was transferred
        vector <asset> quantities = balance_itr->quantities;
        bool found_token = false;
        for (asset &token : quantities) {
            if (token.symbol == quantity.symbol) {
                found_token = true;
                token.amount += quantity.amount;
                break;
            }
        }
        check(found_token, "You first need to announce the asset type you're backing using the announcedepo action");

        balances.modify(balance_itr, same_payer, [&](auto &_balance) {
            _balance.quantities = quantities;
        });

    } else {
        check(false, "invalid memo");
    }
}


ACTION atomicassets::logtransfer(
    name collection_name,
    name from,
    name to,
    vector <uint64_t> asset_ids,
    string memo
) {
    require_auth(get_self());

    notify_collection_accounts(collection_name);
}


ACTION atomicassets::lognewoffer(
    uint64_t offer_id,
    name sender,
    name recipient,
    vector <uint64_t> sender_asset_ids,
    vector <uint64_t> recipient_asset_ids,
    string memo
) {
    require_auth(get_self());

    require_recipient(sender);
    require_recipient(recipient);
}


ACTION atomicassets::lognewtempl(
    int32_t template_id,
    name authorized_creator,
    name collection_name,
    name schema_name,
    bool transferable,
    bool burnable,
    uint32_t max_supply,
    ATTRIBUTE_MAP immutable_data
) {
    require_auth(get_self());

    notify_collection_accounts(collection_name);
}


ACTION atomicassets::logmint(
    uint64_t asset_id,
    name authorized_minter,
    name collection_name,
    name schema_name,
    int32_t template_id,
    name new_asset_owner,
    ATTRIBUTE_MAP immutable_data,
    ATTRIBUTE_MAP mutable_data,
    vector <asset> backed_tokens,
    ATTRIBUTE_MAP immutable_template_data
) {
    require_auth(get_self());

    require_recipient(new_asset_owner);

    notify_collection_accounts(collection_name);
}


ACTION atomicassets::logsetdata(
    name asset_owner,
    uint64_t asset_id,
    ATTRIBUTE_MAP old_data,
    ATTRIBUTE_MAP new_data
) {
    require_auth(get_self());

    assets_t owner_assets = get_assets(asset_owner);
    auto asset_itr = owner_assets.find(asset_id);

    notify_collection_accounts(asset_itr->collection_name);
}


ACTION atomicassets::logbackasset(
    name asset_owner,
    uint64_t asset_id,
    asset backed_token
) {
    require_auth(get_self());

    require_recipient(asset_owner);

    assets_t owner_assets = get_assets(asset_owner);
    auto asset_itr = owner_assets.find(asset_id);

    notify_collection_accounts(asset_itr->collection_name);
}


ACTION atomicassets::logburnasset(
    name asset_owner,
    uint64_t asset_id,
    name collection_name,
    name schema_name,
    int32_t template_id,
    vector <asset> backed_tokens,
    ATTRIBUTE_MAP old_immutable_data,
    ATTRIBUTE_MAP old_mutable_data,
    name asset_ram_payer
) {
    require_auth(get_self());

    notify_collection_accounts(collection_name);
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
    vector <uint64_t> asset_ids,
    string memo,
    name scope_payer
) {
    check(is_account(to), "to account does not exist");

    check(from != to, "Can't transfer assets to yourself");

    check(asset_ids.size() != 0, "asset_ids needs to contain at least one id");

    check(memo.length() <= 256, "A transfer memo can only be 256 characters max");

    vector <uint64_t> asset_ids_copy = asset_ids;
    std::sort(asset_ids_copy.begin(), asset_ids_copy.end());
    check(std::adjacent_find(asset_ids_copy.begin(), asset_ids_copy.end()) == asset_ids_copy.end(),
        "Can't transfer the same asset multiple times");

    assets_t from_assets = get_assets(from);
    assets_t to_assets = get_assets(to);

    map <name, vector <uint64_t>> collection_to_assets_transferred = {};

    for (uint64_t asset_id : asset_ids) {
        auto asset_itr = from_assets.require_find(asset_id,
            ("Sender doesn't own at least one of the provided assets (ID: " +
             to_string(asset_id) + ")").c_str());

        //Existence doesn't have to be checked because this always has to exist
        if (asset_itr->template_id >= 0) {
            templates_t collection_templates = get_templates(asset_itr->collection_name);

            auto template_itr = collection_templates.find(asset_itr->template_id);
            check(template_itr->transferable,
                ("At least one asset isn't transferable (ID: " + to_string(asset_id) + ")").c_str());
        }

        //This is needed for sending notifications later
        if (collection_to_assets_transferred.find(asset_itr->collection_name) !=
            collection_to_assets_transferred.end()) {
            collection_to_assets_transferred[asset_itr->collection_name].push_back(asset_id);
        } else {
            collection_to_assets_transferred[asset_itr->collection_name] = {asset_id};
        }

        //to assets are empty => no scope has been created yet
        bool no_previous_scope = to_assets.begin() == to_assets.end();
        if (no_previous_scope) {
            //A dummy asset is emplaced, which makes the scope_payer pay for the ram of the scope
            //This asset is later deleted again.
            //This action will therefore fail is the scope_payer didn't authorize the action
            to_assets.emplace(scope_payer, [&](auto &_asset) {
                _asset.asset_id = ULLONG_MAX;
                _asset.collection_name = name("");
                _asset.schema_name = name("");
                _asset.template_id = -1;
                _asset.ram_payer = scope_payer;
                _asset.backed_tokens = {};
                _asset.immutable_serialized_data = {};
                _asset.mutable_serialized_data = {};
            });
        }

        to_assets.emplace(asset_itr->ram_payer, [&](auto &_asset) {
            _asset.asset_id = asset_itr->asset_id;
            _asset.collection_name = asset_itr->collection_name;
            _asset.schema_name = asset_itr->schema_name;
            _asset.template_id = asset_itr->template_id;
            _asset.ram_payer = asset_itr->ram_payer;
            _asset.backed_tokens = asset_itr->backed_tokens;
            _asset.immutable_serialized_data = asset_itr->immutable_serialized_data;
            _asset.mutable_serialized_data = asset_itr->mutable_serialized_data;
        });

        from_assets.erase(asset_itr);

        if (no_previous_scope) {
            to_assets.erase(to_assets.find(ULLONG_MAX));
        }
    }

    //Sending notifications
    for (const auto&[collection, assets_transferred] : collection_to_assets_transferred) {
        action(
            permission_level{get_self(), name("active")},
            get_self(),
            name("logtransfer"),
            make_tuple(collection, from, to, assets_transferred, memo)
        ).send();
    }
}


/**
*  The specified asset is backed by the specified quantitiy.
*  This is done in an internal function because it is needed both in the mintasset and the backasset action
*/
void atomicassets::internal_back_asset(
    name payer,
    name asset_owner,
    uint64_t asset_id,
    asset token_to_back
) {
    check(token_to_back.amount > 0, "token_to_back must be positive");

    //The internal_decrease_balance function will throw if payer does not have a sufficient balance
    internal_decrease_balance(payer, token_to_back);

    assets_t owner_assets = get_assets(asset_owner);
    auto asset_itr = owner_assets.require_find(asset_id,
        "The specified owner does not own the asset with the specified ID");

    if (asset_itr->template_id != -1) {
        templates_t collection_templates = get_templates(asset_itr->collection_name);

        auto template_itr = collection_templates.find(asset_itr->template_id);
        check(template_itr->burnable, "The asset is not burnable. Only burnable assets can be backed.");
    }

    vector <asset> backed_tokens = asset_itr->backed_tokens;
    bool found_backed_token = false;
    for (asset &token : backed_tokens) {
        if (token.symbol == token_to_back.symbol) {
            found_backed_token = true;
            token.amount += token_to_back.amount;
            break;
        }
    }
    if (!found_backed_token) {
        backed_tokens.push_back(token_to_back);
    }

    owner_assets.modify(asset_itr, payer, [&](auto &_asset) {
        _asset.ram_payer = payer;
        _asset.backed_tokens = backed_tokens;
    });

    action(
        permission_level{get_self(), name("active")},
        get_self(),
        name("logbackasset"),
        make_tuple(asset_owner, asset_id, token_to_back)
    ).send();
}


/**
*  Decreases the balance of a specified account by a specified quantity
*  If the specified account does not have at least as much tokens in the balance as should be removed
*  the transaction will fail
*/
void atomicassets::internal_decrease_balance(
    name owner,
    asset quantity
) {
    auto balance_itr = balances.require_find(owner.value,
        "The specified account does not have a balance table row");

    vector <asset> quantities = balance_itr->quantities;
    bool found_token = false;
    for (auto itr = quantities.begin(); itr != quantities.end(); itr++) {
        if (itr->symbol == quantity.symbol) {
            found_token = true;
            check(itr->amount >= quantity.amount,
                "The specified account's balance is lower than the specified quantity");
            itr->amount -= quantity.amount;
            if (itr->amount == 0) {
                quantities.erase(itr);
            }
            break;
        }
    }
    check(found_token,
        "The specified account does not have a balance for the symbol specified in the quantity");

    //Updating the balances table
    if (quantities.size() > 0) {
        balances.modify(balance_itr, same_payer, [&](auto &_balance) {
            _balance.quantities = quantities;
        });
    } else {
        balances.erase(balance_itr);
    }
}


/**
* Notifies all of a collection's notify accounts using require_recipient
*/
void atomicassets::notify_collection_accounts(
    name collection_name
) {
    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    for (const name &notify_account : collection_itr->notify_accounts) {
        require_recipient(notify_account);
    }
}


/**
* Checks if the account_to_check is in the authorized_accounts vector of the specified collection
*/
void atomicassets::check_has_collection_auth(
    name account_to_check,
    name collection_name,
    string error_message
) {
    auto collection_itr = collections.require_find(collection_name.value,
        "No collection with this name exists");

    check(std::find(
        collection_itr->authorized_accounts.begin(),
        collection_itr->authorized_accounts.end(),
        account_to_check
        ) != collection_itr->authorized_accounts.end(),
        error_message);
}


/**
* The "name" attribute is limited to 64 characters max for both assets and collections
* This function checks that, if there exists an ATTRIBUTE with name: "name", the value of it
* must be of length <= 64
*/
void atomicassets::check_name_length(
    ATTRIBUTE_MAP data
) {
    auto data_itr = data.find("name");
    if (data_itr != data.end()) {
        if (std::holds_alternative <string>(data_itr->second)) {
            check(std::get <string>(data_itr->second).length() <= 64,
                "Names (attribute with name: \"name\") can only be 64 characters max");
        }
    }
}


atomicassets::assets_t atomicassets::get_assets(name acc) {
    return assets_t(get_self(), acc.value);
}


atomicassets::schemas_t atomicassets::get_schemas(name collection_name) {
    return schemas_t(get_self(), collection_name.value);
}


atomicassets::templates_t atomicassets::get_templates(name collection_name) {
    return templates_t(get_self(), collection_name.value);
}