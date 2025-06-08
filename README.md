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

