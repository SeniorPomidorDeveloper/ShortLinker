# ShortLinker

Асинхронный микросервис для сокращения URL, построенный на фреймворке [userver](https://github.com/userver-framework/userver) на C++.

## 📋 Требования

### Зависимости для локальной сборки

```bash
sudo apt-get update
sudo apt-get install -y \
    cmake \
    ninja-build \
    pkg-config \
    git \
    libpq-dev \
    postgresql-client \
    libldap2-dev \
    libboost-all-dev \
    libyaml-cpp-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    zlib1g-dev
```

## 🌐 API документация

### Создание короткой ссылки

**Endpoint:** `POST /shorten`

**Описание:** Создает новую коротку ссылку для переданного URL

**Request:**
```bash
curl -X POST http://localhost:8080/shorten \
  -H "Content-Type: application/json" \
  -d '{"url":"https://example.com/very/long/path/to/page"}'
```

**Request Body:**
```json
{
  "url": "https://example.com/very/long/path/to/page"
}
```

**Response (201 Created):**
```json
{
  "id": "aBc12DeF",
  "short_url": "http://localhost:8080/r/aBc12DeF"
}
```

**Возможные ошибки:**
- `400 Bad Request` - невалидный JSON или отсутствует поле `url`
- `400 Bad Request` - поле `url` пустое

---

### Редирект по короткой ссылке

**Endpoint:** `GET /r/{id}`

**Описание:** Перенаправляет на оригинальный URL

**Request:**
```bash
curl -L http://localhost:8080/r/aBc12DeF
```

**Response (302 Found):**
```
Location: https://example.com/very/long/path/to/page
```

**Возможные ошибки:**
- `404 Not Found` - короткая ссылка не найдена

---

### Примеры использования

#### Python (requests)
```python
import requests

# Создание ссылки
response = requests.post('http://localhost:8080/shorten', 
    json={'url': 'https://example.com/long-page'})
result = response.json()
print(f"Короткая ссылка: {result['short_url']}")

# Следование по ссылке
response = requests.get(result['short_url'], allow_redirects=True)
print(f"Целевой URL: {response.url}")
```

### 1. Клонирование репозитория

```bash
git clone https://github.com/SeniorPomidorDeveloper/ShortLinker/ 
cd ShortLinker
git submodule update --init --recursive
```

### 2. Сборка с помощью Makefile (рекомендуется)

#### Полная сборка (Debug + Release + тесты)
```bash
make all
```

#### Сборка Debug версии
```bash
make build-debug
```

#### Сборка Release версии
```bash
make build-release
```

#### Запуск тестов
```bash
# Debug тесты
make test-debug

# Release тесты
make test-release

# Оба варианта
make test-debug test-release
```

#### Запуск сервиса
```bash
# Debug версия
make start-debug

# Release версия
make start-release
```

#### Очистка build файлов
```bash
# Очистить конкретный preset
make clean-debug
make clean-release

# Очистить всё
make dist-clean
```

### 3. CMake команды (продвинутые)

#### Конфигурация CMake
```bash
# Debug с sanitizers (ASAN/UBSAN)
cmake --preset debug

# Release без оптимизаций
cmake --preset release

# Кастомный preset
cmake --preset debug-custom -DCMAKE_CXX_FLAGS="-O3"
```

#### Сборка
```bash
cmake --build build-debug -j$(nproc) --target ShortLinker
cmake --build build-release -j$(nproc) --target ShortLinker
```

## 📦 Сборка Debian пакета

### Локально

```bash
# Установить зависимости для сборки пакета
sudo apt-get install -y devscripts debhelper dpkg-dev

# Соберите пакет
dpkg-buildpackage -b -us -uc

# Результат будет в родительской директории
ls -lh ../*.deb
```

### Через GitHub Actions

Пакет автоматически собирается при:
- Push в ветку `dev`, `main` или `feature/**`
- Pull request
- Ручной запуск workflow

**Release пакет:** При merge в `main` автоматически создается GitHub Release с `.deb` файлом.

Скачать `.deb` пакет:
```bash
# Из GitHub Release
wget https://github.com/SeniorPomidorDeveloper/ShortLinker/releases/download/v1.0.0/shortlinker_1.0.0-1_amd64.deb

# Установить
sudo dpkg -i shortlinker_1.0.0-1_amd64.deb
```

## 📥 Установка сервиса

### Из Debian пакета

```bash
sudo dpkg -i shortlinker_1.0.0-1_amd64.deb

# Проверить установку
shortlinker --help
```

### Локально из build директории

```bash
./build-debug/ShortLinker

# Или с параметрами
./build-release/ShortLinker --config configs/static_config.yaml
```

## ⚙️ Конфигурация

### Основной конфиг-файл
```yaml
# configs/static_config.yaml
server:
  listener:
    port: 8080
    
logging:
  level: debug

postgresql:
  connection_string: "postgresql://user:password@localhost:5432/shortlinker"
```

### Переменные окружения
```bash
PORT=8080
POSTGRESQL_CONNECTION_STRING="postgresql://localhost/shortlinker"
LOG_LEVEL=info
```

## 🗄️ База данных

### Подготовка PostgreSQL

```bash
# Создать пользователя и БД
sudo -u postgres psql <<EOF
CREATE USER shortlinker WITH PASSWORD 'password';
CREATE DATABASE shortlinker OWNER shortlinker;
GRANT ALL ON DATABASE shortlinker TO shortlinker;
EOF

# Или для локального development
createdb shortlinker
```

### Схема таблиц

```sql
CREATE TABLE short_links (
    id TEXT PRIMARY KEY,
    target_url TEXT NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
);
```

Описание полей:
- `id` - уникальный идентификатор ссылки (8 символов, случайные буквы/цифры)
- `target_url` - оригинальный URL, на который ведет короткая ссылка
- `created_at` - дата создания короткой ссылки

### Применение миграций

Миграции находятся в `postgresql/migrations/` и автоматически применяются при:
- Запуске сервиса (`make start-debug`)
- Запуске тестов (`make test-debug`)

Миграции используют Flyway версионирование:
- `V001__create_short_links.sql` - создание таблицы short_links

Ручное применение миграций:
```bash
psql shortlinker -f postgresql/migrations/V001__create_short_links.sql
```

## 🧪 Тестирование

### Unit тесты (GTest)
```bash
make test-debug
# Результаты в build-debug/Testing/
```

### Функциональные тесты (pytest)
```bash
# Автоматически запускаются при: make test-debug

# Или вручную
cd build-debug/testsuite
python3 -m pytest tests/ -v
```

### Benchmark тесты
```bash
./build-release/ShortLinker_benchmark
```

## 🐳 Docker

### Сборка Docker образа (если используется)
```bash
docker build -t shortlinker:latest .
docker run -p 8080:8080 shortlinker:latest
```

## 📊 Структура проекта

```
ShortLinker/
├── src/
│   ├── main.cpp              # Entry point
│   └── handlers/             # HTTP handlers
├── tests/
│   ├── unit/                 # GTest unit tests
│   ├── benchmark/            # Google Benchmark
│   └── conftest.py           # pytest конфиг
├── configs/
│   ├── static_config.yaml    # Основная конфигурация
│   └── config_vars.yaml      # Переменные конфига
├── postgresql/
│   ├── migrations/           # Database migrations
│   └── queries/              # SQL queries
├── .github/workflows/        # GitHub Actions CI/CD
│   ├── ci.yml               # CI тесты
│   ├── docker.yaml          # Docker сборка
│   └── build-deb.yml        # DEB пакет сборка
├── debian/                   # Debian packaging
├── CMakeLists.txt           # CMake конфигурация
├── CMakePresets.json        # CMake presets
└── Makefile                 # Build targets
```

## 🔧 Разработка

### Форматирование кода
```bash
make format
```

### Сборка в Docker контейнере
```bash
docker run -it \
  -v $(pwd):/mnt \
  ghcr.io/userver-framework/ubuntu-22.04-userver-pg-dev \
  bash -c "cd /mnt && make all"
```

## 👨‍💻 Разработка и CI/CD

### GitHub Actions Workflows

| Workflow | Триггер | Действие |
|----------|---------|---------|
| **docker.yaml** | Push в любую ветку | Сборка и тесты в Docker контейнере (ubuntu-22.04-userver-pg-dev) |
| **ci.yml** | Push в `dev` | CI матрица тестов на чистой Ubuntu 24.04 (debug + release) |
| **build-deb.yml** | Push в любую + PR | Сборка DEB пакета |

## 🔧 Локальное разработка

### Запуск сервиса в dev режиме

```bash
# 1. Убедитесь что PostgreSQL запущена
psql -U shortlinker -d shortlinker -c "SELECT 1"

# 2. Запустите сервис
make start-debug
```

Сервис будет запущен на `http://localhost:8080`

### Development конфигурация

Запустите с эксплицитным конфиг-файлом:
```bash
./build-debug/ShortLinker --config configs/static_config.yaml
```

Горячая перезагрузка (требует изменения кода):
```bash
# В первом терминале
make start-debug

# В другом при изменении кода
make build-debug
# Перезагрузится автоматически через testsuite
```

## 📚 Дополнительные ресурсы

- [userver документация](https://userver.readthedocs.io/)
- [PostgreSQL driver для userver](https://userver.readthedocs.io/en/latest/postgresql_service.html)
- [CMake документация](https://cmake.org/cmake/help/latest/)
- [GitHub Actions документация](https://docs.github.com/en/actions)

## 📝 Лицензия

Смотри [LICENSE](LICENSE) файл