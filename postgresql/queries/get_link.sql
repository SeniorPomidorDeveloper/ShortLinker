-- @name get_link
-- Get target URL by short link id
SELECT target_url
FROM short_links
WHERE id = $1
