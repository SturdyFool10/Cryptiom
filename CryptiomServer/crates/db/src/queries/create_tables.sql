CREATE TABLE IF NOT EXISTS users (
    username VARCHAR PRIMARY KEY,
    password_hash VARCHAR NULL,
    password_salt VARCHAR NULL,
    public_key VARCHAR NULL,
    security_key VARCHAR UNIQUE NULL
);
