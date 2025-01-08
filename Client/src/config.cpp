#include "config.hpp"
#include <sstream>
#include <expected>
#include "cereal/archives/json.hpp"
using std::string;
using Config::config;

std::expected<config, string> Config::parse_config(string configStr) {
  config cfg;
  std::stringstream ss(configStr);
  try {
    cereal::JSONInputArchive archive(ss);
    archive(cereal::make_nvp("window", cfg.window));
    return cfg;
  } catch (const exception &e) {
    return std::unexpected(string("Failed to parse config: ") + e.what());
  }
}

std::expected<config, string> Config::loadConfig(string path) {
  ifstream file(path);
  if (!file.is_open()) {
    // File doesn't exist, create with defaults
    ofstream newFile(path);
    if (!newFile.is_open()) {
      return std::unexpected("Failed to create config file");
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
      auto cfg = parse_config(ss.str());
      if (!cfg) {
        return std::unexpected(cfg.error());
      }
      return cfg;
    } catch (const exception &e) {
      return std::unexpected(
          string("Failed to parse default config: ") + e.what());
    }
  }

  // File exists, read and parse it
  string configStr((istreambuf_iterator<char>(file)),
                   istreambuf_iterator<char>());

  try {
    auto res = parse_config(configStr);
    if (!res) {
      return std::unexpected(res.error());
    }
    return res;
  } catch (const exception &e) {
    return std::unexpected(string("Failed to parse config file: ") +
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
