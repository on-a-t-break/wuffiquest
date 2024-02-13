#pragma once

void wuffiquest::usecraft(uint64_t & craft_index, eosio::name & owner)
{
    require_auth(owner);

    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.require_find(craft_index, "Craft Index not found!");

    eosio::check(eosio::current_time_point().sec_since_epoch() >= crafts_itr->time, "Craft will be available in " + 
        std::to_string(crafts_itr->time - eosio::current_time_point().sec_since_epoch()) + " Seconds");

    auto rolls_ = get_rolls();

    rolls_.emplace(owner, [&](auto &row){
        row.index = rolls_.available_primary_key();
        row.owner = owner;
        row.balance = (eosio::asset){(uint64_t)0, crafts_itr->cost.quantity.symbol};
        row.craft_index = craft_index;
        row.roll_count = 0;
        row.rolled = false;
    });

    crafts_.modify(crafts_itr, get_self(), [&](auto & row){
        row.uses += 1;
    });

}


void wuffiquest::token_deposit(eosio::name & from, eosio::name & to, eosio::asset & quantity, std::string & memo)
{
    if (to != get_self() || from == get_self() || memo == "testing"){
        return;
    }

    auto rolls_ = get_rolls();
    auto rolls_itr = rolls_.require_find((uint64_t)std::stoi(memo), "Roll not found!");

    eosio::check(rolls_itr->owner == from, "You do not own this Roll!");
    eosio::check(rolls_itr->rolled == false, "Cannot deposit tokens when RNG seed has already been rolled!");

    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.require_find(rolls_itr->craft_index, "Craft Index not found!");

    eosio::check(crafts_itr->cost.contract == get_first_receiver(), "Invalid Token Contract!");
    eosio::check(crafts_itr->cost.quantity.amount == quantity.amount, "Invalid Balance Deposit!");

    rolls_.modify(rolls_itr, eosio::same_payer, [&](auto &row){
        row.balance.amount = quantity.amount;
    });

    size_t size = eosio::transaction_size();
    char buf[size];
    uint32_t read = eosio::read_transaction(buf, size);
    eosio::check(size == read, "Signing value generation: read_transaction() has failed.");
    eosio::checksum256 tx_id = eosio::sha256(buf, read);
    uint64_t signing_value;
    memcpy(&signing_value, tx_id.data(), sizeof(signing_value));

    while (orng::signvals.find(signing_value) != orng::signvals.end())
    {
        signing_value++;
    }

    eosio::action(eosio::permission_level{get_self(), eosio::name("active")}, orng::ORNG_CONTRACT,
                    eosio::name("requestrand"),
                    std::make_tuple(rolls_itr->index,
                                    signing_value, get_self()))
        .send();

}



void wuffiquest::performroll(uint64_t & index, eosio::name & owner)
{
    require_auth(owner);

    auto rolls_ = get_rolls();
    auto rolls_itr = rolls_.require_find(index, "Roll Index not found!");

    eosio::check(rolls_itr->owner == owner, "Invalid Owner!");

    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.require_find(rolls_itr->craft_index, "Craft Index not found!");

    eosio::check(rolls_itr->rolled == true, "RNG Seed has not yet been received!");

    RandomnessProvider randomness_provider(rolls_itr->seed);

    auto coin_flip_odds = crafts_itr->odds;

    // 0 -> Min
    // 1 -> Max
    // 2 -> Iterations
    // 3 -> Success
    // 4 -> Out Of

    int64_t result = 1;

    while (result < coin_flip_odds[1]){
        auto success = randomness_provider.get_rand((uint32_t)coin_flip_odds[3]);
        if (success < (uint32_t)coin_flip_odds[4]){
            result += coin_flip_odds[2];
        } else {
            break;
        }
    }

    rolls_.modify(rolls_itr, eosio::same_payer, [&](auto & row){
        row.roll_count = result;
    });

    
}

void wuffiquest::updatescore(uint64_t & index, eosio::name & owner)
{
    require_auth(owner);

    auto rolls_ = get_rolls();
    auto rolls_itr = rolls_.require_find(index, "Roll Index not found!");

    auto crafts_ = get_crafts();
    auto crafts_itr = crafts_.require_find(rolls_itr->craft_index, "Craft Index not found!");

    eosio::check(rolls_itr->owner == owner, "Invalid Owner!");
    eosio::check(rolls_itr->rolled == true && rolls_itr->roll_count >= 1, "Roll has not been performed yet!");

    auto leaderboard_ = get_leaderboard();
    auto leaderboard_itr = leaderboard_.find(owner.value);

    if (leaderboard_itr != leaderboard_.end()){
        leaderboard_.modify(leaderboard_itr, eosio::same_payer, [&](auto & row){
            row.owner = owner;
            row.craft_index = rolls_itr->craft_index;
            row.roll_count = rolls_itr->roll_count;
        });
    } else {
        leaderboard_.emplace(owner, [&](auto & row){
            row.owner = owner;
            row.craft_index = rolls_itr->craft_index;
            row.roll_count = rolls_itr->roll_count;
        });
    }
}