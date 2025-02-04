use rusqlite::{params, Connection, Result};
struct Db {
    conn: Connection,
}

impl Db {
    pub fn init(db_path: &str) -> Result<Self> {
        let conn = Connection::open(db_path)?;
        let sql = include_str!("../queries/create_tables.sql");
        conn.execute_batch(sql)?;
        Ok(Self { conn })
    }

    // Perform security key login check
    pub fn login_security_key(&self, key: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/login_security_key.sql"))?;
        let exists: i32 = stmt.query_row(params![key], |row| row.get(0))?;
        Ok(exists == 1)
    }

    // Perform login using username, password, and security key (two-factor authentication)
    pub fn login_user_password_tfa(&self, username: &str, password: &str, security_key: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/login_u_p_s_two_factor.sql"))?;
        let exists: i32 = stmt.query_row(params![username, password, security_key], |row| row.get(0))?;
        Ok(exists == 1)
    }

    // Retrieve stored salt for a user
    pub fn ret_salt(&self, username: &str) -> Result<Option<String>> {
        let mut stmt = self.conn.prepare(include_str!("../queries/ret_salt.sql"))?;
        let result = stmt.query_row(params![username], |row| row.get(0))?;
        Ok(result)
    }

    pub fn security_key_login(&self, security_key: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/security_key_login.sql"))?;
        let row: String = stmt.query_row(params![security_key], |row| row.get(0))?;
        if !row.is_empty() {
            if row == security_key.to_string() {
                Ok(true)
            } else {
                Ok(false)
            }
        } else {
            Ok(false)
        }
    }

    pub fn security_key_on_file(&self, security_key: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/security_key_on_file.sql"))?;
        let row: String = stmt.query_row(params![security_key], |row| row.get(0))?;
        if !row.is_empty() {
            if row == security_key.to_string() {
                Ok(true)
            } else {
                Ok(false)
            }
        } else {
            Ok(false)
        }
    }

    pub fn user_exists(&self, username: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/user_exists.sql"))?;
        let row: i32 = stmt.query_row(params![username], |row| row.get(0))?;
        Ok(row > 0)
    }

    pub fn user_has_security_key(&self, username: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/user_has_security_key.sql"))?;
        let row: i32 = stmt.query_row(params![username], |row| row.get(0))?;
        Ok(row > 0)
    }

    pub fn username_password_login(&self, username: &str, password_hash: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/username_password_login.sql"))?;
        let row = stmt.query_row(params![username, password_hash], |row| row.get(0))?;
        Ok(row > 0)
    }

    pub fn create_user(&self, username: &str, password_hash: &str, salt: &str) -> Result<bool> {
        let res = self.user_exists(username)?;
        if res {
            Err("that username already exists".into())
        } else {
            let mut stmt = self.conn.prepare(include_str!("../queries/create_user.sql"))?;
            let rows = stmt.execute(params![username, password_hash, salt])?;
            if rows > 0 {
                Ok(true)
            } else {
                Ok(false)
            }
        }
    }

    pub fn delete_user(&self, username: &str, password_hash: &str) -> Result<bool> {
        let res = self.user_exists(username)?;
        if res {
            let mut stmt = self.conn.prepare(include_str!("../queries/remove_user.sql"))?;
            let rows_changed = stmt.execute(params![username, password_hash])?;
            Ok(rows_changed > 0)
        } else {
            Err("No such user".into())
        }
    }

    pub fn remove_user_ban(&self, username: &str) -> Result<bool> {
        let res = self.user_exists(username)?;
        if res {
            let mut stmt = self.conn.prepare(include_str!("../queries/remove_user_ban.sql"))?;
            let rows_changed = stmt.execute(params![username])?;
            Ok(rows_changed > 0)
        } else {
            Err("No such user".into())
        }
    }
}

pub fn connect_file(path: &str) -> Db {
    let conn = Connection::open(path).unwrap();
    Db { conn }
}