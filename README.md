# PathfinderQt

Тестовый проект на C++/Qt для поиска пути на сетке.  
Используется алгоритм BFS (поиск в ширину).

## Возможности
- Генерация сетки с указанием ширины, высоты и плотности стен
- Установка начальной (A) и конечной (B) точки левой кнопкой мыши
- Установка стенки правой кнопкой мыши
- Поиск кратчайшего пути и подсветка маршрута
- Подсветка промежуточного пути при наведении курсора

## Сборка

### Требования
- Qt 6 (модули Core, Gui, Widgets)
- CMake 3.16+
- MSVC (Visual Studio 2019 или новее)

### Инструкции для Windows (MSVC)
```bash
git clone https://github.com/Des7r/PathfinderQt.git
cd PathfinderQt
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH="E:/Qt/6.8.1/msvc2019_64"
cmake --build . --config Release
```

### Запуск
После сборки исполняемый файл будет лежать здесь:
```bash
build/Release/PathfinderQt.exe
```
Чтобы приложение запускалось вне Qt Creator, нужно собрать вместе с ним все нужные Qt-библиотеки.  
Проще всего это сделать через утилиту `windeployqt` (идёт вместе с Qt):
```bash
cd build/Release
windeployqt PathfinderQt.exe
```
