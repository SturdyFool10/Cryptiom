SELECT password_hash, password_salt, security_key
FROM users
WHERE username = $1 AND security_key = $3 AND password_hash = $2;
