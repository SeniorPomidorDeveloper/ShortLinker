import pathlib

import pytest

from testsuite.databases.pgsql import discover


pytest_plugins = [
    'pytest_userver.plugins.postgresql',
]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    databases = discover.find_schemas(
        'service2_13',
        [service_source_dir / 'postgresql' / 'migrations'],
    )
    return pgsql_local_create(list(databases.values()))


@pytest.fixture(scope='session')
def service_source_dir():
    return pathlib.Path(__file__).parent.parent
