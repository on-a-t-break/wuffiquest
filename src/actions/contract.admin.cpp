#pragma once

void wuffiquest::addcraft(uint64_t & craft_index, std::string & name, eosio::extended_asset & cost, std::vector<int64_t> odds, uint64_t & uses, bool & active, uint32_t & time)
{
    require_auth(get_self());
    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.find(craft_index);
    if (crafts_itr != crafts_.end()){
        crafts_.modify(crafts_itr, get_self(), [&](auto &row){
            row.name = name;
            row.cost = cost;
            row.odds = odds;
            row.uses = uses;
            row.active = active;
            row.time = time;
        }); 
    } else {
        crafts_.emplace(get_self(), [&](auto &row){
            row.craft_index = crafts_.available_primary_key();
            row.name = name;
            row.cost = cost;
            row.odds = odds;
            row.uses = uses;
            row.active = active;
            row.time = time;
        });
    }
}



void wuffiquest::rmvcraft(uint64_t & craft_index)
{
    require_auth(get_self());

    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.find(craft_index);
    if (crafts_itr != crafts_.end()){
        crafts_.erase(crafts_itr);
    }
}




void wuffiquest::activecraft(uint64_t & craft_index, bool & active)
{
    require_auth(get_self());

    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.find(craft_index);
    if (crafts_itr != crafts_.end()){
        crafts_.modify(crafts_itr, get_self(), [&](auto &row){
            row.active = active;
        }); 
    }
}
