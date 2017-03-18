#pragma once

#include "handlers/exchange_handler.h"
#include "exchanges/okcoin_futs.h"

class OKCoinFutsHandler : public ExchangeHandler {
public:
  OKCoinFutsHandler(std::string, std::shared_ptr<Config>, OKCoinFuts::ContractType);

  void set_up_and_start() override;
  void manage_positions(double) override;

  boost::optional<OKCoinFuts::UserInfo> userinfo();
  // open a limit that closes after some time
  bool limit(OKCoinFuts::OrderType, double, int, double, std::chrono::seconds);

  std::shared_ptr<OKCoinFuts> okcoin_futs;
  OKCoinFuts::ContractType contract_type;
};