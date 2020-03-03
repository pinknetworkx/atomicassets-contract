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
title: Extend collections scheme
summary: 'Extends the scheme to serialize collection data by one or more lines'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The following FORMAT lines are added to the scheme that is used to serialize collections data:
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




<h1 class="contract">createscheme</h1>

---
spec_version: "0.2.0"
title: Create a scheme
summary: '{{nowrap authorized_creator}} creates a new scheme with the name {{nowrap scheme_name}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_creator}} creates a new scheme with the name {{scheme_name}}. This scheme belongs to the collection {{collection_name}}

{{#if scheme_format}}The scheme will be initialized with the following FORMAT lines that can be used to serialize preset and asset data:
    {{#each scheme_format}}
        - name: {{this.name}} , type: {{this.type}}
    {{/each}}
{{else}}The scheme will be initialized without any FORMAT lines.
{{/if}}

Only authorized accounts of the {{collection_name}} collection will be able to extend the scheme by adding additional FORMAT lines in the future, but they will not be able to delete previously added FORMAT lines.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_creator}}.

{{authorized_creator}} has to be an authorized account in the collection {{collection_name}}.

Creating schemes with the purpose of confusing or taking advantage of others, especially by impersonating other well known brands, personalities or dapps is not allowed.
</div>




<h1 class="contract">extendscheme</h1>

---
spec_version: "0.2.0"
title: Extend scheme
summary: 'Extends the scheme {{nowrap scheme_name}} by adding one or more FORMAT lines'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
The scheme {{scheme_name}} belonging to the collection {{collection_name}} is extended by adding the following FORMAT lines that can be used to serialize preset and asset data:
{{#each scheme_format_extension}}
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

{{#if authorized_accounts}}The following accounts are added to the authorized_accounts list, allowing them create and edit presets and assets within this collection:
    {{#each authorized_accounts}}
        - {{this}}
    {{/each}}
{{else}}No accounts are added to the authorized_accounts list.
{{/if}}

{{#if notify_accounts}}The following accounts are added to the notify_accounts list, which means that they get notified on the blockchain of any actions related to assets and presets of this collection:
    {{#each notify_accounts}}
        - {{this}}
    {{/each}}
{{else}}No accounts are added to the notify_accounts list.
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

This allows {{account_to_add}} to both create and edit presets and assets of this collection.
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

This removes {{account_to_remove}}'s permission to both create and edit presets and assets of this collection.
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
- A preset of the collection {{collection_name}} is created
- A preset of the collection {{collection_name}} has its mutable data changed

{{account_to_add}} is able to add code to their own smart contract to handle these notifications. 
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of the collection's author.
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




<h1 class="contract">createpreset</h1>

---
spec_version: "0.2.0"
title: Create a preset
summary: '{{nowrap authorized_creator}} creates a new preset which belongs to the {{nowrap collection_name}} collection and uses the {{nowrap scheme_name}} scheme'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_creator}} creates a new preset which belongs to the {{collection_name}} collection.

The scheme {{scheme_name}} is used for the serialization of the preset's data.

{{#if transferable}}The assets within this preset will be transferable
{{else}}The assets within this preset will not be transferable
{{/if}}

{{#if burnable}}The assets within this preset will be burnable
{{else}}The assets within this preset will not be burnable
{{/if}}

{{#if max_supply}}A maximum of {{max_supply}} assets can ever be created within this preset.
{{else}}There is no maximum amount of assets that can be created within this preset.
{{/if}}

{{#if immutable_data}}The immutable data of the preset is set to:
    {{#each immutable_data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}No immutable data is set for the preset.
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_creator}}.

{{authorized_creator}} has to be an authorized account in the collection {{collection_name}}.
</div>




<h1 class="contract">mintasset</h1>

---
spec_version: "0.2.0"
title: Mint an asset
summary: '{{nowrap authorized_minter}} mints an asset which will be owned by {{nowrap new_owner}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{authorized_minter}} mints an asset of the preset which belongs to the {{scheme_name}} scheme of the {{collection_name}} collection. The asset will be owned by {{new_owner}}.

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
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_minter}}.

{{authorized_minter}} has to be an authorized account in the collection that the preset with the id {{preset_id}} belongs to.

Minting assets with the purpose of confusing or taking advantage of others, especially by impersonating other well known brands, personalities or dapps is not allowed.

Minting assets with the purpose of spamming or otherwise negatively impacing {{new_owner}} is not allowed.
</div>




<h1 class="contract">setassetdata</h1>

---
spec_version: "0.2.0"
title: Set the mutable data of an asset
summary: '{{nowrap authorized_editor}} sets the mutable data of the asset with the id {{nowrap asset_id}} owned by {{nowrap owner}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{#if new_mutable_data}}{{authorized_editor}} sets the mutable data of the asset with the id {{asset_id}} owned by {{nowrap owner}} to the following:
    {{#each new_mutable_data}}
        - name: {{this.key}} , value: {{this.value}}
    {{/each}}
{{else}}{{authorized_editor}} clears the mutable data of the asset with the id {{asset_id}} owned by {{owner}}.
{{/if}}
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{authorized_editor}}.

{{authorized_editor}} has to be an authorized account in the collection that the asset with the id {{asset_id}} belongs to. (An asset belongs to the collection that the preset it is within belongs to)
</div>




<h1 class="contract">burnasset</h1>

---
spec_version: "0.2.0"
title: Burn an asset
summary: '{{nowrap owner}} burns his asset with the id {{nowrap asset_id}}'
icon: https://atomicassets.io/image/logo256.png#108AEE3530F4EB368A4B0C28800894CFBABF46534F48345BF6453090554C52D5
---

<b>Description:</b>
<div class="description">
{{owner}} burns his asset with the id {{asset_id}}.

If there previously were core tokens backed for this asset, these core tokens are transferred to {{owner}}.
</div>

<b>Clauses:</b>
<div class="clauses">
This action may only be called with the permission of {{owner}}.
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