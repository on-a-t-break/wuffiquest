#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>


namespace eosiotoken
{
struct account
{
    eosio::asset balance;

    uint64_t primary_key() const
    {
        return balance.symbol.code().raw();
    }
};

struct currency_stats
{
    eosio::asset supply;
    eosio::asset max_supply;
    eosio::name issuer;

    uint64_t primary_key() const
    {
        return supply.symbol.code().raw();
    }
};

typedef eosio::multi_index<eosio::name("accounts"), account> accounts;
typedef eosio::multi_index<eosio::name("stat"), currency_stats> stats;

static eosio::asset get_balance(const eosio::name &token_contract_account, const eosio::name &owner,
                                const eosio::symbol_code &sym_code)
{
    accounts accountstable(token_contract_account, owner.value);
    const auto &ac = accountstable.require_find(sym_code.raw(), "Token balance not setup");
    return ac->balance;
}

static uint64_t get_supply(const eosio::name &token_contract_account, const eosio::symbol_code &sym_code)
{
    stats statstable(token_contract_account, sym_code.raw());
    const auto &stat = statstable.require_find(sym_code.raw(), "Token does not exist in token contract");
    return stat->supply.amount;
}

static uint64_t get_max_supply(const eosio::name &token_contract_account, const eosio::symbol_code &sym_code)
{
    stats statstable(token_contract_account, sym_code.raw());
    const auto &stat = statstable.require_find(sym_code.raw(), "Token does not exist in token contract");
    return stat->max_supply.amount;
}
} // namespace eosiotoken
