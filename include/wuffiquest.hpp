#pragma once

#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/transaction.hpp>

#include <headers/RandomnessProvider.hpp>
#include <contracts/orng.wax.hpp>




CONTRACT wuffiquest : public eosio::contract
{
  public:
    using eosio::contract::contract;


// RNG Structures

    struct cfg_params
    {   
        eosio::extended_asset allowed_token;

    };


// ACTIONS

// Contract Actions
    [[eosio::action]] void trackinit(std::string & memo);
    [[eosio::action]] void trackdest(std::string & memo);
    [[eosio::action]] void trackset(eosio::asset & tokens_burnt, eosio::asset & tokens_total, double & distribution);

    [[eosio::action]] void cfginit(std::string & memo);
    [[eosio::action]] void cfgdestruct(std::string & memo);
    [[eosio::action]] void cfgsetparams(cfg_params & params, std::string & memo);

// RNG
    [[eosio::action]] void receiverand(uint64_t index, eosio::checksum256 random_value);

// Admin Actions
    [[eosio::action]] void addcraft(uint64_t & craft_index, std::string & name, eosio::extended_asset & cost, std::vector<int64_t> odds, uint64_t & uses, bool & active, uint32_t & time);
    [[eosio::action]] void rmvcraft(uint64_t & craft_index);
    [[eosio::action]] void activecraft(uint64_t & craft_index, bool & active);

// User Actions
    [[eosio::action]] void usecraft(uint64_t & craft_index, eosio::name & owner);
    [[eosio::action]] void performroll(uint64_t & index, eosio::name & owner);
    [[eosio::action]] void updatescore(uint64_t & index, eosio::name & owner);


// Notifications
    [[eosio::on_notify("*::transfer")]] void token_deposit(eosio::name & from, eosio::name & to, eosio::asset & quantity, std::string & memo);


// PRIVATE
  private:


// Tracker Table
    struct [[eosio::table("tracker")]] _tracker_entity
    {
        eosio::asset tokens_burnt;
        eosio::asset tokens_total;
        double distribution;
    };

    typedef eosio::singleton<eosio::name("tracker"), _tracker_entity> _tracker;
    _tracker get_tracker() {return _tracker(get_self(), get_self().value);}

// Crafts Table
    struct [[eosio::table("crafts")]] _crafts_entity
    {
        uint64_t craft_index;
        std::string name;
        eosio::extended_asset cost;
        std::vector<int64_t> odds;
        uint64_t uses;
        bool active;
        uint32_t time;

        uint64_t primary_key()  const{return (uint64_t)craft_index;};
    };
    typedef eosio::multi_index<eosio::name("crafts"), _crafts_entity> _crafts;
    _crafts get_crafts() {return _crafts(get_self(), get_self().value);}


// Rolls Table

    struct [[eosio::table("rolls")]] _rolls_entity
    {
        uint64_t index;
        eosio::name owner;
        eosio::asset balance;
        uint64_t craft_index;
        uint64_t roll_count;

        bool rolled;
        eosio::checksum256 seed;

        uint64_t primary_key() const{return (uint64_t)index;};
        uint64_t secondary_key() const{return (uint64_t)owner.value;};
    };
    typedef eosio::multi_index<eosio::name("rolls"), _rolls_entity,
        eosio::indexed_by<eosio::name("owners"), eosio::const_mem_fun<_rolls_entity, uint64_t, &_rolls_entity::secondary_key>>> _rolls;
    _rolls get_rolls() {return _rolls(get_self(), get_self().value);}

// Rolls Table

    struct [[eosio::table("leaderboard")]] _leaderboard_entity
    {
        eosio::name owner;
        uint64_t craft_index;
        uint64_t roll_count;

        uint64_t primary_key() const{return (uint64_t)owner.value;};
        uint64_t secondary_key() const{return (uint64_t)roll_count;};
    };

    typedef eosio::multi_index<eosio::name("leaderboard"), _leaderboard_entity,
        eosio::indexed_by<eosio::name("rollcount"), eosio::const_mem_fun<_leaderboard_entity, uint64_t, &_leaderboard_entity::secondary_key>>> _leaderboard;
    _leaderboard get_leaderboard() {return _leaderboard(get_self(), get_self().value);}

// Config Table
    struct [[eosio::table("config")]] _config_entity
    {
        cfg_params params;
    };

    typedef eosio::singleton<eosio::name("config"), _config_entity> _config;
    _config get_config() {return _config(get_self(), get_self().value);}


};