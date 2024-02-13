#include <eosio/eosio.hpp>

namespace orng
{
static constexpr eosio::name ORNG_CONTRACT = eosio::name("orng.wax");

struct [[eosio::table]] signvals_a
{
    uint64_t signing_value;

    uint64_t primary_key() const
    {
        return signing_value;
    }
};
typedef eosio::multi_index<eosio::name("signvals.a"), signvals_a> signvals_t;

signvals_t signvals = signvals_t(ORNG_CONTRACT, ORNG_CONTRACT.value);
}