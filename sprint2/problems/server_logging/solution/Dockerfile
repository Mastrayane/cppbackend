# Создаём образ для сборки
FROM gcc:11.3 as build

# Устанавливаем необходимые зависимости
RUN apt update && \
    apt install -y \
      python3-pip \
      cmake \
    && \
    pip3 install conan==1.*

# Копируем файлы проекта в контейнер
COPY conanfile.txt /app/
COPY ./src /app/src
COPY CMakeLists.txt /app/

# Создаем директорию для сборки
RUN mkdir /app/build

# Переходим в директорию сборки
WORKDIR /app/build

# Устанавливаем зависимости для каждой конфигурации
RUN conan install .. --build=missing -s build_type=Debug
RUN conan install .. --build=missing -s build_type=Release
RUN conan install .. --build=missing -s build_type=RelWithDebInfo
RUN conan install .. --build=missing -s build_type=MinSizeRel

# Выполняем сборку для каждой конфигурации
RUN cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
RUN cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && cmake --build .
RUN cmake -DCMAKE_BUILD_TYPE=MinSizeRel .. && cmake --build .

# Второй контейнер для запуска
FROM ubuntu:22.04 as run

# Создадим пользователя www
RUN groupadd -r www && useradd -r -g www www
USER www

# Скопируем приложение со сборочного контейнера в директорию /app.
# Не забываем также папку data, она пригодится.
COPY --from=build /app/build/bin/game_server /app/
COPY ./data /app/data

# Запускаем игровой сервер
ENTRYPOINT ["/app/game_server", "/app/data/config.json"]