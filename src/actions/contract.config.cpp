#pragma once

void wuffiquest::trackinit(std::string & memo)
{
    require_auth(get_self());
    get_tracker().set(_tracker_entity{}, get_self());
}

void wuffiquest::trackdest(std::string & memo)
{
    require_auth(get_self());
    get_tracker().remove();
}

void wuffiquest::trackset(eosio::asset & tokens_burnt, eosio::asset & tokens_total, double & distribution)
{
    require_auth(get_self());
    auto _tracker = get_tracker();
    auto new_tracker = _tracker.get();

    new_tracker.tokens_burnt = tokens_burnt;
    new_tracker.tokens_total = tokens_total;
    new_tracker.distribution = distribution;

    _tracker.set(new_tracker, get_self());
}


void wuffiquest::cfginit(std::string &memo)
{
    require_auth(get_self());
    get_config().set(_config_entity{}, get_self());

}

void wuffiquest::cfgdestruct(std::string &memo)
{
    require_auth(get_self());
    get_config().remove();

}

void wuffiquest::cfgsetparams(cfg_params & params, std::string &memo)
{
    require_auth(get_self());

    auto _config = get_config();
    auto new_config = _config.get();

    new_config.params = params;

    _config.set(new_config, get_self());
}







