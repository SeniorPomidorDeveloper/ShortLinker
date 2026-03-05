-- @name insert_link
-- Insert a short link
INSERT INTO short_links (id, target_url)
VALUES ($1, $2)
ON CONFLICT (id) DO NOTHING
RETURNING id
