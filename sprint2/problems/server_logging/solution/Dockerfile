# Создаём образ для сборки на Windows
FROM mcr.microsoft.com/windows/servercore:ltsc2019 as build_windows

# Устанавливаем необходимые зависимости
RUN powershell -Command \
    Invoke-WebRequest -Uri https://bootstrap.pypa.io/get-pip.py -OutFile get-pip.py; \
    python get-pip.py; \
    pip install conan==1.*; \
    Remove-Item get-pip.py

# Копируем файлы проекта в контейнер
COPY conanfile.txt /app/
COPY ./src /app/src
COPY CMakeLists.txt /app/

# Создаем директорию для сборки
RUN mkdir /app/build

# Переходим в директорию сборки
WORKDIR /app/build

# Устанавливаем зависимости для каждой конфигурации на Windows
RUN conan install .. --build=missing -s build_type=Debug -s compiler.runtime=MD
RUN conan install .. --build=missing -s build_type=Release -s compiler.runtime=MT
RUN conan install .. --build=missing -s build_type=RelWithDebInfo -s compiler.runtime=MT
RUN conan install .. --build=missing -s build_type=MinSizeRel -s compiler.runtime=MT

# Выполняем сборку для каждой конфигурации
RUN cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .
RUN cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
RUN cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && cmake --build .
RUN cmake -DCMAKE_BUILD_TYPE=MinSizeRel .. && cmake --build .

# Создаём образ для сборки на Linux
FROM gcc:11.3 as build_linux

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

# Устанавливаем зависимости для каждой конфигурации на Linux
RUN conan install .. --build=missing -s build_type=Release -s compiler.libcxx=libstdc++11
RUN conan install .. --build=missing -s build_type=Debug -s compiler.libcxx=libstdc++11

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
COPY --from=build_linux /app/build/bin/game_server /app/
COPY ./data /app/data

# Запускаем игровой сервер
ENTRYPOINT ["/app/game_server", "/app/data/config.json"]