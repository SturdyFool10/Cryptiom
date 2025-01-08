#include <string>
#include <fstream>
#include <expected>



//shortening section
using std::string;
using std::expected;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::exception;





namespace Config {
    struct WindowConfig {
        int width;
        int height;
        bool vsync;
        bool antiAliasing;

        template<class Archive>
        void serialize(Archive & archive);

        constexpr static auto Default() {
            return WindowConfig{800, 600, true, true};
        }
    };
    class config {
        //TODO build config class
        public:
        WindowConfig window;

        constexpr static auto Default() {
            return config{WindowConfig::Default()};
        }
    };

    expected<config, string> parse_config(string configStr);

    expected<config, string> loadConfig(string path);
}
