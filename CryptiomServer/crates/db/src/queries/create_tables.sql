-- table below stores users, banned users will remain in this
CREATE TABLE IF NOT EXISTS users (
    username VARCHAR PRIMARY KEY,
    password_hash VARCHAR not NULL,
    password_salt VARCHAR not NULL,
    public_key VARCHAR NULL,
    security_key VARCHAR UNIQUE NULL
);
-- table below is for banned users, users here have been banned by a server admin
CREATE TABLE IF NOT EXISTS bans (
    username VARCHAR PRIMARY KEY,
    password_hash VARCHAR not NULL,
    password_salt VARCHAR not NULL,
    public_key VARCHAR NULL,
    security_key VARCHAR UNIQUE NULL

);
CREATE TABLE IF NOT EXISTS ip_bans (
    ip_address INET PRIMARY KEY,
    username VARCHAR NOT NULL REFERENCES bans(username) ON DELETE CASCADE,
    banned_on TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    expires_on TIMESTAMP DEFAULT (DATETIME('now', '+1 year'))
);

CREATE TABLE IF NOT EXISTS user_logins (
    username VARCHAR NOT NULL REFERENCES users(username) ON DELETE CASCADE,
    ip_address INET NOT NULL,
    login_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (username, ip_address, login_time)
);
