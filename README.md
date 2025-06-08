# zeta-dirichlet

## Сборка проекта

### Под Linux:

Зависимости и рекомендуемые версии:
* Flint 3 - 3.2.2
* CMake - 3.28.3
* Make - 4.3
* Pkg-config - 1.8.1
* wxWidgets - 3.2.4

Команда для установки всех зависимостей: `sudo apt-get install libflint-dev cmake make pkg-config libwxgtk3.2-dev`

Если wxWidgets не работает, то нужно: 
```sh
sudo apt-get install libwxbase3.2-0-unofficial \
                libwxbase3.2unofficial-dev \
                libwxgtk3.2-0-unofficial \
                libwxgtk3.2unofficial-dev \
                wx3.2-headers \
                wx-common \
                libwxgtk-media3.2-0-unofficial \
                libwxgtk-media3.2unofficial-dev \
                libwxgtk-webview3.2-0-unofficial \ 
                libwxgtk-webview3.2unofficial-dev \ 
                libwxgtk-webview3.2-0-unofficial-dbg \ 
                libwxbase3.2-0-unofficial-dbg \
                libwxgtk3.2-0-unofficial-dbg \
                libwxgtk-media3.2-0-unofficial-dbg \
                wx3.2-i18n \
                wx3.2-examples
```

Flint 3 либо устанавливается из менеджера пакетов, либо собирается вручную по
инструкциям из [репозитория flint](https://github.com/flintlib/flint/tree/flint-3.1).

Для сборки проекта используется CMake:

```sh
mkdir build
cd build
cmake ../
cmake --build .
```

### Кросс-компилция под Windows:

Требуется тулчейн mingw-w64-x86-64.

```sh
cmake -DCMAKE_TOOLCHAIN_FILE=mingw64.cmake -DCMAKE_BUILD_TYPE=Release -B build-win64 -S .
cd build-win64 && make -j $(nproc)
```

## Сборка конспекта лекций

* Установить зависимости
```sh
sudo apt install texlive texlive-base texlive-lang-cyrillic
```

* Собрать всё целиком...
```sh
make
```

* ... или, например, собрать только страницу "нулевой встречи"
```sh
make docs/meeting0.pdf
```

