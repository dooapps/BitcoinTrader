#pragma once

#include "../include/log.h"
#include "../include/okcoin.h"
#include "../include/strategies.h"
#include "../include/mktdata.h"
#include "../include/config.h"

using stops_t = std::vector<std::shared_ptr<Stop>>;

class BitcoinTrader {
public:
  BitcoinTrader(std::shared_ptr<Config>);
  ~BitcoinTrader();

  // interactive commands
  void reconnect() { exchange->reconnect = true; }
  std::string status();

  // interfaces to Exchange
  // (declared public because may be used interactively)
  void cancel_order(std::string);

  // start the exchange
  // after setting callbacks and creating log files
  void start();

protected:
  // source of data and market actions
  // is dynamic because on reconnects old one is thrown out
  // and new one allocated
  std::shared_ptr<Exchange> exchange;

  // private config options
  // sourced from config file
  std::shared_ptr<Config> config;

  // used for logging trading actions
  std::shared_ptr<Log> trading_log;
  // used for logging exchange actions
  std::shared_ptr<Log> exchange_log;

  // if this is being destructed
  bool done;

  // vector of threads performing some recurring actions
  // used for destructor
  std::vector<std::shared_ptr<std::thread>> running_threads;

  // used to check if the exchange is working
  void check_connection();

  // used to continually fetch user information
  // currently just btc and cny
  void fetch_userinfo();
  double user_btc;
  double user_cny;

  // updated in real time to latest tick
  Ticker tick;

  // map of OHLC bars together with indicator values
  // keyed by period in minutes they represent
  std::map<std::chrono::minutes, std::shared_ptr<MktData>> mktdata;

  // held until trade and orderinfo callbacks are complete
  // to order market events
  std::mutex execution_lock;

  std::vector<std::shared_ptr<Strategy>> strategies;

  // live stops
  stops_t stops;
  // used so that the price for take profit limits is calculated
  // at the same time (and near the same code) as stops
  double tp_limit;
  // stops waiting to be added (ie, limit order waiting to be filled)
  stops_t pending_stops;

  // called every tick
  void handle_stops();

  // bools used to order subscribing to websocket channels in order
  bool received_userinfo;
  bool received_a_tick;

  // set up the exchange callbacks
  void setup_exchange_callbacks();

  // EXECUTION ALGORITHMS
  // functions to set trade and orderinfo callbacks
  // that lock and unlock execution_lock
  
  // generic market buy / sell amount of BTC
  void market_buy(double, std::function<void(double, double, long)> = nullptr);
  void market_sell(double, std::function<void(double, double, long)> = nullptr);

  // limit order that will cancel after some seconds
  // and after those seconds will run callback given
  // (to set take-profits / stop-losses)
  void limit_buy(double, double, std::chrono::seconds, std::function<void()> = nullptr);
  void limit_sell(double, double, std::chrono::seconds, std::function<void()> = nullptr);
  void limit_algorithm(std::chrono::seconds, std::function<void()> = nullptr);

  // good-til-cancelled limit orders
  // will run callback after receiving order_id
  void GTC_buy(double, double, std::function<void(std::string)> = nullptr);
  void GTC_sell(double, double, std::function<void(std::string)> = nullptr);

  // functions and data relating to limit execution
  // currently will hold a limit for some seconds then cancel
  // it if not filled in time
  std::string current_limit;
  bool done_limit_check;
  double filled_amount;
};
