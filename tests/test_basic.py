# Start via `make test-debug` or `make test-release`


async def test_shorten_and_redirect(service_client):
    # Create a short link
    response = await service_client.post(
        '/shorten',
        json={'url': 'https://example.com/long-page'},
    )
    assert response.status == 201
    data = response.json()
    assert 'id' in data
    assert 'short_url' in data
    link_id = data['id']

    # Follow the short link
    response = await service_client.get(
        f'/r/{link_id}',
        allow_redirects=False,
    )
    assert response.status == 302
    assert response.headers['Location'] == 'https://example.com/long-page'


async def test_redirect_not_found(service_client):
    response = await service_client.get(
        '/r/nonexist',
        allow_redirects=False,
    )
    assert response.status == 404


async def test_shorten_missing_url(service_client):
    response = await service_client.post('/shorten', json={})
    assert response.status == 400


async def test_shorten_empty_url(service_client):
    response = await service_client.post('/shorten', json={'url': ''})
    assert response.status == 400


async def test_shorten_invalid_json(service_client):
    response = await service_client.post(
        '/shorten',
        data='not json',
    )
    assert response.status == 400
