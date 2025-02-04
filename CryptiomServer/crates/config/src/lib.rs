use serde::{Deserialize, Serialize};
#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct Config {
    network: NetworkingConfig,
    db: DatabaseConfig
}

#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct NetworkingConfig {
    interface: String,
    port: u16,
}


#[derive(Serialize, Deserialize, Clone, Debug)]
pub struct DatabaseConfig {
    use_internal: bool,
    url: Option<String>
}

pub fn load_config(path: &str) -> Result<Config, String> {
    if fs::check_file_exists(path) {
        let res = fs::read_file(path)?;
        serde_json::from_str(&res).map_err(|e| e.to_string())
    } else {
        let default = include_str!("defaultconfigserver.json");
        serde_json::from_str(&default).map_err(|e| e.to_string())
    }
}
