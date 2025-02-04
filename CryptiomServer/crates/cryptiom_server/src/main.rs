use config::{load_config, Config};
use pretty_logs::init_logging;
use tracing::*;

#[tokio::main]
async fn main() {
    init_logging();
    let conf_res = load_config("config.json");
    let config: Config;
    match conf_res {
        Ok(v) => config = v,
        Err(e) => {
            error!("[FATAL] Error loading config: {}", e);
            panic!("Error loading config: {}", e);
        }
    }
}
