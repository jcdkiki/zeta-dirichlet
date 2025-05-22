# zeta-dirichlet

## Сборка проекта

Зависимости:
* Flint 3
* CMake
* Make
* Pkg-config
* wxWidgets
* ???

Flint 3 либо устанавливается из менеджера пакетов, либо собирается вручную по
инструкциям из [репозитория flint](https://github.com/flintlib/flint/tree/flint-3.1).

Для сборки проекта используется CMake:

```sh
mkdir build
cd build
cmake ../
cmake --build .
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

