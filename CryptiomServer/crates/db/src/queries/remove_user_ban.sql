-- Step 1: Copy user credentials to bans
INSERT INTO bans (username, password_hash, password_salt, public_key, security_key)
SELECT username, password_hash, password_salt, public_key, security_key
FROM users
WHERE username = $1;

-- Step 2: Remove the user from the users table
DELETE FROM users WHERE username = $1;

-- Step 3: Insert the user's last 30 days' IPs into ip_bans
INSERT INTO ip_bans (ip_address, username, banned_on, expires_on)
SELECT DISTINCT ip_address, $1, DATETIME('now'), DATETIME('now', '+1 year')
FROM user_logins
WHERE username = $1
  AND login_time >= DATETIME('now', '-30 days');
