use std::time::{Duration, Instant, SystemTime};
use rusqlite::{params, Connection, Result};

/// **Database Struct**
///
/// This struct represents a connection to the SQLite database.
/// It encapsulates the `rusqlite::Connection` and provides methods for interacting with the database.
struct Db {
    conn: Connection,
}

/// **Enum for Table Names**
///
/// This enum is used to reference different tables in the database.
/// It allows for type-safe table name retrieval when working with SQL queries.
pub enum Table {
    Users,
    Bans,
    UserLogins,
    BannedIPs
}

/// **Convert Table Enum to String**
///
/// Converts the `Table` enum to a corresponding table name as a `String`.
fn table_to_string(val: Table) -> String {
    match val {
        Table::Users => "users".into(),
        Table::Bans => "bans".into(),
        Table::UserLogins => "user_logins".into(),
        Table::BannedIPs => "ip_bans".into(),
    }
}

/// **User Struct**
///
/// Represents a user in the system.
/// This struct is used when retrieving user data but **excludes sensitive information**
/// such as passwords for security reasons.
pub struct User {
    name: String,
    public_key: String,
    password_hash: String,
    salt: String,
}

/// **User Login Struct**
///
/// Represents a login attempt by a user, including the username and the login time.
pub struct UserLogin {
    username: String,
    time: Instant,
}

/// **Banned IP Struct**
///
/// Represents an IP that has been banned, including the username it was associated with,
/// when the ban was applied, and when it will expire.
pub struct BannedIPs {
    username: String,
    banned_on: Instant,
    expiration: Instant
}

impl Db {
    /// **Initialize Database**
    ///
    /// This function initializes the database by loading a schema from an external SQL file
    /// and executing it. It ensures that the database tables exist before use.
    ///
    /// - `db_path`: Path to the SQLite database file.
    /// - **Returns:** `Db` struct encapsulating the database connection.
    pub fn init(db_path: &str) -> Result<Self> {
        let conn = Connection::open(db_path)?;
        let sql = include_str!("../queries/create_tables.sql");
        conn.execute_batch(sql)?;
        Ok(Self { conn })
    }

    /// **Login using a Security Key**
    ///
    /// This function verifies whether a security key exists in the database.
    ///
    /// - `key`: Security key provided by the user.
    /// - **Returns:** `true` if the security key exists, otherwise `false`.
    pub fn login_security_key(&self, key: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/login_security_key.sql"))?;
        let exists: i32 = stmt.query_row(params![key], |row| row.get(0))?;
        Ok(exists == 1)
    }

    /// **Login using Username, Password, and Security Key (2FA)**
    ///
    /// This function performs a two-factor authentication check.
    ///
    /// - `username`: User's username.
    /// - `password`: User's password hash.
    /// - `security_key`: User's security key.
    /// - **Returns:** `true` if credentials are correct, otherwise `false`.
    pub fn login_user_password_tfa(&self, username: &str, password: &str, security_key: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/login_u_p_s_two_factor.sql"))?;
        let exists: i32 = stmt.query_row(params![username, password, security_key], |row| row.get(0))?;
        Ok(exists == 1)
    }

    /// **Retrieve User's Password Salt**
    ///
    /// - `username`: The username whose salt is being retrieved.
    /// - **Returns:** The stored password salt or `None` if the user doesn't exist.
    pub fn ret_salt(&self, username: &str) -> Result<Option<String>> {
        let mut stmt = self.conn.prepare(include_str!("../queries/ret_salt.sql"))?;
        let result = stmt.query_row(params![username], |row| row.get(0))?;
        Ok(result)
    }

    /// **Check if a User Exists**
    ///
    /// - `username`: Username to check.
    /// - **Returns:** `true` if the user exists, otherwise `false`.
    pub fn user_exists(&self, username: &str) -> Result<bool> {
        let mut stmt = self.conn.prepare(include_str!("../queries/user_exists.sql"))?;
        let row: i32 = stmt.query_row(params![username], |row| row.get(0))?;
        Ok(row > 0)
    }

    /// **Get All Users**
    ///
    /// Retrieves all users from the `users` table, excluding sensitive information.
    pub fn get_all_users(&self) -> Result<Vec<User>> {
        let mut stmt = self.conn.prepare("SELECT username, password_hash, password_salt, public_key FROM users")?;
        let users = stmt.query_map([], |row| {
            Ok(User {
                name: row.get(0)?,
                password_hash: row.get(1)?,
                salt: row.get(2)?,
                public_key: row.get(3)?,
            })
        })?;

        Ok(users.collect::<Result<Vec<_>, _>>()?)
    }

    /// **Get All Banned Users**
    ///
    /// Retrieves all banned users from the `bans` table, excluding sensitive information.
    pub fn get_banned_users(&self) -> Result<Vec<User>> {
        let mut stmt = self.conn.prepare("SELECT username, password_hash, password_salt, public_key FROM bans")?;
        let users = stmt.query_map([], |row| {
            Ok(User {
                name: row.get(0)?,
                password_hash: row.get(1)?,
                salt: row.get(2)?,
                public_key: row.get(3)?,
            })
        })?;

        Ok(users.collect::<Result<Vec<_>, _>>()?)
    }

    /// **Get All User Logins**
    ///
    /// Retrieves all login attempts recorded in `user_logins`,
    /// converting timestamps into `Instant`.
    pub fn get_user_logins(&self) -> Result<Vec<UserLogin>> {
        let mut stmt = self.conn.prepare("SELECT username, login_time FROM user_logins")?;
        let logins = stmt.query_map([], |row| {
            let time: String = row.get(1)?;
            Ok(UserLogin {
                username: row.get(0)?,
                time: Self::convert_to_instant(time),
            })
        })?;

        Ok(logins.collect::<Result<Vec<_>, _>>()?)
    }

    /// **Get All Banned IPs**
    ///
    /// Retrieves all banned IP addresses from the `ip_bans` table.
    pub fn get_banned_ips(&self) -> Result<Vec<BannedIPs>> {
        let mut stmt = self.conn.prepare("SELECT username, banned_on, expires_on FROM ip_bans")?;
        let bans = stmt.query_map([], |row| {
            let banned_on: String = row.get(1)?;
            let expiration: String = row.get(2)?;
            Ok(BannedIPs {
                username: row.get(0)?,
                banned_on: Self::convert_to_instant(banned_on),
                expiration: Self::convert_to_instant(expiration),
            })
        })?;

        Ok(bans.collect::<Result<Vec<_>, _>>()?)
    }

    /// **Convert SQLite Timestamp (TEXT) to Rust `Instant`**
    ///
    /// This function parses a SQLite timestamp and converts it into a Rust `Instant`.
    /// If parsing fails, it returns the current time as a fallback.
    fn convert_to_instant(timestamp: String) -> Instant {
        if let Ok(time) = timestamp.parse::<i64>() {
            let since_epoch = SystemTime::UNIX_EPOCH + Duration::from_secs(time as u64);
            let now = SystemTime::now();
            if let Ok(duration) = now.duration_since(since_epoch) {
                return Instant::now() - duration;
            }
        }
        Instant::now()
    }
}