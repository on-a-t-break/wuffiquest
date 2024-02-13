[[eosio::action]] void wuffiquest::receiverand(uint64_t index, eosio::checksum256 random_value)
{
    eosio::check(has_auth(get_self()) || has_auth(orng::ORNG_CONTRACT), "not authorized");

    auto rolls_ = get_rolls();
    auto rolls_itr = rolls_.require_find(index, "Roll Index not found!");

    rolls_.modify(rolls_itr, eosio::same_payer, [&](auto &row){
        row.rolled = true;
        row.seed = random_value;
    });


}