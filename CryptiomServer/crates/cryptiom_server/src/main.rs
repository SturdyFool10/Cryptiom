use tokio::task::JoinHandle;
use config::{load_config, Config};
use state::AppState;
use pretty_logs::init_logging;
use tracing::*;
use macros::spawn_tasks;
use webserver::start_server;

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
    let state = AppState::new(config);
    let handles: Vec<JoinHandle<_>> = spawn_tasks!(state.clone(), start_server);

}
