#include "../include/exchange_utils.h"

using std::chrono::milliseconds;
using std::chrono::seconds;
using std::function;
using std::this_thread::sleep_for;
using namespace std::chrono_literals;

bool check_until(std::function<bool()> test, std::chrono::nanoseconds stop_time, std::chrono::milliseconds time_between_checks) {
  bool complete = false;
  bool completed_on_time = true;
  do {
    // if we're over time (and our stop_time isn't 0)
    if (timestamp_now() > stop_time &&
        stop_time != 0ns) {
      completed_on_time = false;
      complete = true;
    }
    // if we're under time (or our stop_time is 0)
    else {
      if (test())
        complete = true;
      else
        std::this_thread::sleep_for(time_between_checks);
    }
  } while (!complete);

  return completed_on_time;
}

long optionally_to_long(nlohmann::json object) {
  if (object.is_string()) {
    return stol(object.get<std::string>());
  }
  else
    return object;
}

double optionally_to_double(nlohmann::json object) {
  if (object.is_string()) {
    return stod(object.get<std::string>());
  }
  else
    return object;
}

int optionally_to_int(nlohmann::json object) {
  if (object.is_string()) {
    return stoi(object.get<std::string>());
  }
  else
    return object;
}

std::string dtos(double n, int digits) {
  std::ostringstream n_ss;
  n_ss << std::fixed << std::setprecision(digits) << n;
  return n_ss.str();
}

double truncate_to(double to_round, int digits) {
  int i = to_round * std::pow(10, digits);
  double to_return = i / std::pow(10, digits);
  return to_return;
}

// use this function for consistent timestamps
std::chrono::nanoseconds timestamp_now() {
  return std::chrono::high_resolution_clock::now().time_since_epoch();
}

size_t Curl_write_callback(void *contents, size_t size, size_t nmemb, std::string *s)
{
  size_t newLength = size*nmemb;
  size_t oldLength = s->size();

  try {
      s->resize(oldLength + newLength);
  }
  catch(std::bad_alloc &e) {
      return 0;
  }

  std::copy((char*) contents,
      (char*) contents + newLength,
      s->begin() + oldLength);

  return size*nmemb;
}

std::string curl_post(std::string url, std::string post_fields) {
  CURL *curl;
  CURLcode res;

  std::string output;
  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    if (!post_fields.empty())
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK)
      std::cout << curl_easy_strerror(res) << std::endl;

    curl_easy_cleanup(curl);
  } 
  return output;
}

