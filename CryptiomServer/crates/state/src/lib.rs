use config::Config;
use std::sync::Arc;
use tokio::sync::Mutex;

#[derive(Clone, Debug)]
struct AppState {
    config: Arc<Mutex<Config>>,
    running: Arc<Mutex<bool>>,
}
