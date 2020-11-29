<h1 class="contract">init</h1>

---
spec_version: "0.2.0"
title: Initialize config tables
summary: 'Initialize the tables "config" and "tokenconfig" if they have not been initialized before'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
Initialize the tables "config" and "tokenconfig" if they have not been initialized before. If they have been initialized before, nothing will happen.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{$action.account}}.
</div>




<h1 class="contract">admincoledit</h1>

---
spec_version: "0.2.0"
title: Extend collections schema
summary: 'Extends the schema to serialize collection data by one or more lines'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The following FORMAT lines are added to the schema that is used to serialize collections data:
{{#each collection_format_extension}}
    - name: {{this.name}} , type: {{this.type}}
{{/each}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{$action.account}}.
</div>




<h1 class="contract">setversion</h1>

---
spec_version: "0.2.0"
title: Set tokenconfig version
summary: 'Sets the version in the tokenconfigs table to {{nowrap new_version}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---
<b>Description:</b>
<div class="description">
The version in the tokenconfigs table is set to {{new_version}}.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{$action.account}}.
</div>




<h1 class="contract">addconftoken</h1>

---
spec_version: "0.2.0"
title: Add token to supported list
summary: 'Adds a token that can then be used to back assets'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---
<b>Description:</b>
<div class="description">
The token with the symbol {{token_symbol}} from the token contract {{token_contract}} is added to the supported_tokens list.

This means that assets can then be backed with that specific token.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{$action.account}}.
</div>




<h1 class="contract">transfer</h1>

---
spec_version: "0.2.0"
title: Transfer Assets
summary: 'Send one or more assets from {{nowrap from}} to {{nowrap to}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{from}} transfers one or more assets with the following ids to {{to}}:
{{#each asset_ids}}
    - {{this}}
{{/each}}

{{#if memo}}There is a memo attached to the transfer stating:
    {{memo}}
{{else}}No memo is attached to the transfer.
{{/if}}

If {{to}} does not own any assets, {{from}} pays the RAM for the scope of {{to}} in the assets table.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{from}}.

Transfers that do not serve any purpose other than spamming the recipient are not allowed.
</div>




<h1 class="contract">createschema</h1>

---
spec_version: "0.2.0"
title: Create a schema
summary: '{{nowrap authorized_creator}} creates a new schema with the name {{nowrap schema_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_creator}} creates a new schema with the name {{schema_name}}. This schema belongs to the collection {{collection_name}}

{{#if schema_format}}The schema will be initialized with the following FORMAT lines that can be used to serialize template and asset data:
    {{#each schema_format}}
        - name: {{this.name}} , type: {{this.type}}
    {{/each}}
{{else}}The schema will be initialized without any FORMAT lines.
{{/if}}

Only authorized accounts of the {{collection_name}} collection will be able to extend the schema by adding additional FORMAT lines in the future, but they will not be able to delete previously added FORMAT lines.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_creator}}.

{{authorized_creator}} has to be an authorized account in the collection {{collection_name}}.

Creating schemas with the purpose of confusing or taking advantage of others, especially by impersonating other well known brands, personalities or dapps is not allowed.
</div>




<h1 class="contract">extendschema</h1>

---
spec_version: "0.2.0"
title: Extend schema
summary: 'Extends the schema {{nowrap schema_name}} by adding one or more FORMAT lines'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The schema {{schema_name}} belonging to the collection {{collection_name}} is extended by adding the following FORMAT lines that can be used to serialize template and asset data:
{{#each schema_format_extension}}
    - name: {{this.name}} , type: {{this.type}}
{{/each}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_editor}}.

{{authorized_editor}} has to be an authorized account in the collection {{collection_name}}.
</div>




<h1 class="contract">createcol</h1>

---
spec_version: "0.2.0"
title: Create collection
summary: '{{nowrap author}} creates a new collection with the name {{collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{author}} creates a new collection with the name {{collection_name}}.

{{#if authorized_accounts}}The following accounts are added to the authorized_accounts list, allowing them create and edit templates and assets within this collection:
    {{#each authorized_accounts}}
        - {{this}}
    {{/each}}
{{else}}No accounts are added to the authorized_accounts list.
{{/if}}

{{#if notify_accounts}}The following accounts are added to the notify_accounts list, which means that they get notified on the blockchain of any actions related to assets and templates of this collection:
    {{#each notify_accounts}}
        - {{this}}
    {{/each}}
{{else}}No accounts are added to the notify_accounts list.
{{/if}}

{{#if allow_notify}}It will be possible to add more accounts to the notify_accounts list later.
{{else}}It will not be possible to add more accounts to the notify_accounts list later.
{{/if}}

The market_fee for this collection will be set to {{market_fee}}. 3rd party markets are encouraged to use this value to collect fees for the collection author, but are not required to do so.

{{#if data}}The collections will be initialized with the following data:
    {{#each data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}The collection will be initialized without any data.
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{author}}.

Creating collections with the purpose of confusing or taking advantage of others, especially by impersonating other well known brands, personalities or dapps is not allowed.

If the notify functionality is being used, the notify accounts may not make any transactions throw when receiving the notification. This includes, but is not limited to, purposely blocking certain transfers by making the transaction throw.

It is the collection author's responsibility to enforce that this does not happen.
</div>




<h1 class="contract">setcoldata</h1>

---
spec_version: "0.2.0"
title: Set collection data
summary: 'Sets the data of the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{#if data}}Sets the data of the collection {{collection_name}} to the following
    {{#each data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}Clears the data of the collection {{collection_name}}
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
</div>




<h1 class="contract">addcolauth</h1>

---
spec_version: "0.2.0"
title: Make an account authorized in a collection
summary: 'Add the account {{nowrap account_to_add}} to the authorized_accounts list of the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
Adds the account {{account_to_add}} to the authorized_accounts list of the collection {{collection_name}}.

This allows {{account_to_add}} to both create and edit templates and assets of this collection.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
</div>




<h1 class="contract">remcolauth</h1>

---
spec_version: "0.2.0"
title: Remove an account's authorization in a collection
summary: 'Remove the account {{nowrap account_to_remove}} from the authorized_accounts list of the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
Removes the account {{account_to_remove}} from the authorized_accounts list of the collection {{collection_name}}.

This removes {{account_to_remove}}'s permission to both create and edit templates and assets of this collection.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
</div>




<h1 class="contract">addnotifyacc</h1>

---
spec_version: "0.2.0"
title: Add an account to a collection's notify list
summary: 'Add the account {{nowrap account_to_add}} to the notify_accounts list of the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
Adds the account {{account_to_add}} to the notify_accounts list of the collection {{collection_name}}.

This will make {{account_to_add}} get notifications directly on the blockchain when one of the following actions is performed:
- One or more assets of the collection {{collection_name}} is transferred
- An asset of the collection {{collection_name}} is minted
- An asset of the collection {{collection_name}} has its mutable data changed
- An asset of the collection {{collection_name}} is burned
- An asset of the collection {{collection_name}} gets backed with core tokens
- A template of the collection {{collection_name}} is created

{{account_to_add}} is able to add code to their own smart contract to handle these notifications. 
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.

{{account_to_add}} may not make any transactions throw when receiving a notification. This includes, but is not limited to, purposely blocking certain transfers by making the transaction throw.

It is the collection author's responsibility to enforce that this does not happen.
</div>




<h1 class="contract">remnotifyacc</h1>

---
spec_version: "0.2.0"
title: Remove an account from a collection's notfiy list
summary: 'Remove the account {{nowrap account_to_remove}} from the notify_accounts list of the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
Removes the account {{account_to_remove}} from the notify_accounts list of the collection {{collection_name}}.

{{account_to_remove}} will therefore no longer receive notifications for any of the actions related to the collection {{collection_name}}.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
</div>




<h1 class="contract">setmarketfee</h1>

---
spec_version: "0.2.0"
title: Set collection market fee
summary: 'Sets the market fee of the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The market_fee for the collection {{collection_name}} will be set to {{market_fee}}. 3rd party markets are encouraged to use this value to collect fees for the collection author, but are not required to do so.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
</div>




<h1 class="contract">forbidnotify</h1>

---
spec_version: "0.2.0"
title: Disallow collection notifications
summary: 'Sets the allow_notify value of the collection {{nowrap collection_name}} to false'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The allow_notify value of the collection {{collection_name}} is set to false.
This means that it will not be possible to add accounts to the notify_accounts list later.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
</div>




<h1 class="contract">createtempl</h1>

---
spec_version: "0.2.0"
title: Create a template
summary: '{{nowrap authorized_creator}} creates a new template which belongs to the {{nowrap collection_name}} collection and uses the {{nowrap schema_name}} schema'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_creator}} creates a new template which belongs to the {{collection_name}} collection.

The schema {{schema_name}} is used for the serialization of the template's data.

{{#if transferable}}The assets within this template will be transferable
{{else}}The assets within this template will not be transferable
{{/if}}

{{#if burnable}}The assets within this template will be burnable
{{else}}The assets within this template will not be burnable
{{/if}}

{{#if max_supply}}A maximum of {{max_supply}} assets can ever be created within this template.
{{else}}There is no maximum amount of assets that can be created within this template.
{{/if}}

{{#if immutable_data}}The immutable data of the template is set to:
    {{#each immutable_data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}No immutable data is set for the template.
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_creator}}.

{{authorized_creator}} has to be an authorized account in the collection {{collection_name}}.
</div>




<h1 class="contract">locktemplate</h1>

---
spec_version: "0.2.0"
title: Locks a template
summary: '{{nowrap authorized_editor}} locks the template with the id {{nowrap template_id}} belonging to the collection {{nowrap collection_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_editor}} locks the template with the id {{template_id}} belonging to the collection {{collection_name}}.

This sets the template's maximum supply to the template's current supply, which means that no more assets referencing this template can be minted.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_creator}}.

{{authorized_creator}} has to be an authorized account in the collection {{collection_name}}.

The template's issued supply must be greater than 0.
</div>




<h1 class="contract">mintasset</h1>

---
spec_version: "0.2.0"
title: Mint an asset
summary: '{{nowrap authorized_minter}} mints an asset which will be owned by {{nowrap new_asset_owner}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_minter}} mints an asset of the template which belongs to the {{schema_name}} schema of the {{collection_name}} collection. The asset will be owned by {{new_asset_owner}}.

{{#if immutable_data}}The immutable data of the asset is set to:
    {{#each immutable_data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}No immutable data is set for the asset.
{{/if}}

{{#if mutable_data}}The mutable data of the asset is set to:
    {{#each mutable_data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}No mutable data is set for the asset.
{{/if}}

{{#if quantities_to_back}}The asset will be backed with the following tokens and {{authorized_minter}} needs to have at least that amount of tokens in their balance:
    {{#each quantities_to_back}}
        - {{quantities_to_back}}
    {{/each}}
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_minter}}.

{{authorized_minter}} has to be an authorized account in the collection that the template with the id {{template_id}} belongs to.

Minting assets that contain intellectual property requires the permission of the all rights holders of that intellectual property.

Minting assets with the purpose of confusing or taking advantage of others, especially by impersonating other well known brands, personalities or dapps is not allowed.

Minting assets with the purpose of spamming or otherwise negatively impacing {{new_owner}} is not allowed.
</div>




<h1 class="contract">setassetdata</h1>

---
spec_version: "0.2.0"
title: Set the mutable data of an asset
summary: '{{nowrap authorized_editor}} sets the mutable data of the asset with the id {{nowrap asset_id}} owned by {{nowrap asset_owner}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{#if new_mutable_data}}{{authorized_editor}} sets the mutable data of the asset with the id {{asset_id}} owned by {{nowrap asset_owner}} to the following:
    {{#each new_mutable_data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}{{authorized_editor}} clears the mutable data of the asset with the id {{asset_id}} owned by {{asset_owner}}.
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_editor}}.

{{authorized_editor}} has to be an authorized account in the collection that the asset with the id {{asset_id}} belongs to. (An asset belongs to the collection that the template it is within belongs to)
</div>




<h1 class="contract">announcedepo</h1>

---
spec_version: "0.2.0"
title: Announces a deposit
summary: '{{nowrap owner}} adds the symbol {{nowrap symbol_to_announce}} to his balance table row'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
This action is used to add a zero value asset to the quantities vector of the balance row with the owner {{owner}}.
If there is no balance row with the owner {{owner}}, a new one is created.
Adding something to a vector increases the RAM required, therefore this can't be done directly in the receipt of the transfer action, so using this action a zero value is added so that the RAM required doesn't change when adding the received quantity in the transfer action later.

By calling this action, {{payer}} pays for the RAM of the balance table row with the owner {{owner}}.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{payer}}.
</div>




<h1 class="contract">withdraw</h1>

---
spec_version: "0.2.0"
title: Withdraws fungible tokens
summary: '{{nowrap owner}} withdraws {{token_to_withdraw}} from his balance'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{owner}} withdraws {{token_to_withdraw}} that they previously deposited and have not yet spent otherwise.
The tokens will be transferred back to {{owner}} and will be deducted from {{owner}}'s balance.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{owner}}.
</div>




<h1 class="contract">backasset</h1>

---
spec_version: "0.2.0"
title: Backs an asset with tokens
summary: '{{nowrap payer}} backs the asset with the ID {{nowrap asset_id}} with {{nowrap token_to_back}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{payer}} backs an the asset with the ID {{asset_id}} owned by {{asset_owner}} with {{token_to_back}}.
{{payer}} must have at least as many tokens in his balance. {{token_to_back}} will be removed from {{payer}}'s balance.
The tokens backed to this asset can be retreived by burning the asset, in which case the owner at the time of the burn will receive the tokens.

{{payer}} pays for the full RAM cost of the asset.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{payer}}.
</div>




<h1 class="contract">burnasset</h1>

---
spec_version: "0.2.0"
title: Burn an asset
summary: '{{nowrap asset_owner}} burns his asset with the id {{nowrap asset_id}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{asset_owner}} burns his asset with the id {{asset_id}}.

If there previously were tokens backed to this asset, these tokens are added to the balance table entry {{asset_owner}}.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{asset_owner}}.
</div>




<h1 class="contract">createoffer</h1>

---
spec_version: "0.2.0"
title: Create an offer
summary: '{{nowrap sender}} makes an offer to {{nowrap recipient}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{sender}} makes the following offer to {{recipient}}.

{{#if sender_asset_ids}}{{sender}} gives the assets with the following ids:
    {{#each sender_asset_ids}}
        - {{this}}
    {{/each}}
{{else}}{{sender}} does not give any assets.
{{/if}}

{{#if recipient_asset_ids}}{{recipient}} gives the assets with the following ids:
    {{#each recipient_asset_ids}}
        - {{this}}
    {{/each}}
{{else}}{{recipient}} does not give any assets.
{{/if}}

If {{recipient}} accepts the offer, the assets will automatically be transferred to the respective sides.

{{#if memo}}There is a memo attached to the offer stating:
    {{memo}}
{{else}}No memo is attached to the offer.
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{sender}}.

Creating offers that do not serve any purpose other than spamming the recipient is not allowed.

{{sender}} must not take advantage of the notification they receive when the offer is accepted or declined in a way that harms {{recipient}}.
</div>




<h1 class="contract">canceloffer</h1>

---
spec_version: "0.2.0"
title: Cancel an offer
summary: 'The offer with the id {{nowrap offer_id}} is cancelled'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The creator of the offer with the id {{offer_id}} cancels this offer. The offer is deleted from the offers table.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the creator of the offer.
</div>



<h1 class="contract">acceptoffer</h1>

---
spec_version: "0.2.0"
title: Accept an offer
summary: 'The offer with the id {{nowrap offer_id}} is accepted'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The recipient of the offer with the id {{offer_id}} accepts the offer.

The assets from either side specified in the offer are automatically transferred to the respective other side.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the recipient of the offer.
</div>



<h1 class="contract">declineoffer</h1>

---
spec_version: "0.2.0"
title: Decline an offer
summary: 'The offer with the id {{nowrap offer_id}} is declined'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The recipient of the offer with the id {{offer_id}} declines the offer. The offer is deleted from the offers table.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the recipient of the offer.
</div>



<h1 class="contract">payofferram</h1>

---
spec_version: "0.2.0"
title: Pays RAM for existing offer
summary: '{{nowrap payer}} will pay for the RAM cost of the offer {{nowrap offer_id}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{payer}} pays for the RAM cost of the offer {{offer_id}}. The offer itself is not modified
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{payer}}.
</div>