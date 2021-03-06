#include "handlers/okcoin_spot_handler.h"

using std::lock_guard;
using std::mutex;
using std::shared_ptr;
using std::make_shared;
using std::string;

OKCoinSpotHandler::OKCoinSpotHandler(string name, shared_ptr<Config> config, string exchange_log_key, string trading_log_key) :
    ExchangeHandler(name, config, exchange_log_key, trading_log_key) {
}

void OKCoinSpotHandler::set_up_and_start() {
  okcoin_spot = std::make_shared<OKCoinSpot>(name, exchange_log, config);
  exchange = okcoin_spot;

  auto open_callback = [&]() {
    okcoin_spot->subscribe_to_ticker();
  };
  okcoin_spot->set_open_callback(open_callback);

  auto ticker_callback = [&](const Ticker& new_tick) {
    tick.set(new_tick);
  };
  okcoin_spot->set_ticker_callback(ticker_callback);

  okcoin_spot->start();
}

void OKCoinSpotHandler::reconnect_exchange() {
  exchange_log->output("RECONNECTING TO " + name);

  tick.clear();
  depth.clear();

  okcoin_spot->reconnect();
}