SELECT password_hash, password_salt, security_key
FROM users
WHERE username = $1;
