#include "config.hpp"
#include <sstream>
#include "result.hpp"
#include "cereal/archives/json.hpp"
using std::string;
using result::Result;
using Config::config;



Result<config, string> Config::parse_config(string configStr) {
  config cfg;
  std::stringstream ss(configStr);
  try {
    cereal::JSONInputArchive archive(ss);
    archive(cereal::make_nvp("window", cfg.window));
    return Result<config, string>::Ok(cfg);
  } catch (const exception &e) {
    return Result<config, string>::Err(string("Failed to parse config: ") +
                                       e.what());
  }
}
Result<config, string> Config::loadConfig(string path) {

  ifstream file(path);
  if (!file.is_open()) {
    // File doesn't exist, create with defaults
    ofstream newFile(path);
    if (!newFile.is_open()) {
      return Result<config, string>::Err("Failed to create config file");
    }

    auto defaultConfig = config::Default();
    std::stringstream ss;
    {
        cereal::JSONOutputArchive archive(ss);
        archive(cereal::make_nvp("window", defaultConfig.window));
    }
    newFile << ss.str();
    newFile.close();

    try {
      config cfg =
          parse_config(ss.str()).unwrap();
      return Result<config, string>::Ok(cfg);
    } catch (const exception &e) {
      return Result<config, string>::Err(
          string("Failed to parse default config: ") + e.what());
    }
  }

  // File exists, read and parse it
  string configStr((istreambuf_iterator<char>(file)),
                   istreambuf_iterator<char>());

  try {
    auto res = parse_config(configStr);
    if (res.is_err()) {
      return Result<config, string>::Err(res.unwrap_err());
    } else {
      return Result<config, string>::Ok(res.unwrap());
    }
  } catch (const exception &e) {
    return Result<config, string>::Err(string("Failed to parse config file: ") +
                                       e.what());
  }
}


template<class Archive>
void Config::WindowConfig::serialize(Archive & archive) {
    archive(CEREAL_NVP(width),
           CEREAL_NVP(height),
           CEREAL_NVP(vsync),
           CEREAL_NVP(antiAliasing));
}
