use config::Config;
use std::sync::Arc;
use tokio::sync::Mutex;

#[derive(Clone, Debug)]
pub struct AppState {
    config: Arc<Mutex<Config>>,
    running: Arc<Mutex<bool>>,
}

impl AppState {
    pub fn new(config: Config) -> Self {
        Self {
            config: Arc::new(Mutex::new(config)),
            running: Arc::new(Mutex::new(true)),
        }
    }
}