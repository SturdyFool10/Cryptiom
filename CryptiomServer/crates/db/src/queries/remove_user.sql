DELETE FROM users WHERE username = $1 AND password_hash = $2;
