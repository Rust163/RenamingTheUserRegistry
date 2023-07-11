#include <iostream>
#include<string>
#include <Windows.h>

int main() {
    setlocale(LC_ALL, "rus");
    std::string newFolderName;
    std::cout << "Введите новое название папки пользователя: ";
    std::getline(std::cin, newFolderName);

    std::string oldFolderPath = "C:\\Users\\";
    std::string newFolderPath = "C:\\Users\\" + newFolderName;

    // Изменение названия папки пользователя
    if (MoveFileA(oldFolderPath.c_str(), newFolderPath.c_str())) {
        //С помощью функции MoveFile, перемещаем содержимое из старой дирктории в новую.
        //.c_str()` используется для получения указателя на C-строку, которую ожидает функция `MoveFile`
        std::cout << "Название папки пользователя успешно изменено." << std::endl;

        // Изменение названия папки в реестре\
        //Открывает ключ реестра с папками профилей пользователей
        HKEY hKey;
        LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList", 0, KEY_ALL_ACCESS, &hKey);
        //Используем ключ RegOpenKeyExA(HKEY_LOCAL_MACHINE, -ключ раздела реестра с информацией о локальной машине
        //"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList", -путь к ключу реестра где мы будем менять название папки User при каждом упоминании старого имени
        //   0, -флаг открытия ключа
        //    KEY_ALL_ACCESS, -флаг доступа, в нашем случае доступ полный
        //    &hKey) -указатель на нашу переменную в которой будет храниться результат открытия ключа
        if (result == ERROR_SUCCESS) {
            DWORD dataSize = MAX_PATH;//размер данных значения в реестре
            char buffer[MAX_PATH];//это буфер, в который будет записано значение из реестра
            result = RegQueryValueExA(hKey, "ProfilesDirectory", NULL, NULL, (LPBYTE)buffer, &dataSize);
            //RegQueryValueExA(hKey, -получаем результат значений из реестра, который мы получили ранее
            //"ProfilesDirectory", -запрашиваем значение из открытого ключа
            //NULL, -тип значения записи из реестра не указываем
            //NULL, -размер данных не указываем, потому что он не превышает MAX_PATH
            //(LPBYTE)buffer, -буфер для записи значения из реестра
            //&dataSize); -указатель на переменную которая хранит размер буфера данных и будет обновлена фактическим размером значений из реестра
            if (result == ERROR_SUCCESS) {
                std::string profilesDirectory = buffer;//значение из реестра, полученное в буфере buffer, присваивается переменной profilesDirectory типа string. Таким образом, значение из реестра теперь доступно в виде строки
                std::string oldProfilePath = profilesDirectory + "\\" + newFolderName;
                std::string newProfilePath = profilesDirectory + "\\" + newFolderName;//новый путь профиля в реестре после его переименования
                result = RegSetValueExA(hKey, "ProfilesDirectory", 0, REG_SZ, (LPBYTE)newProfilePath.c_str(), newProfilePath.size());
                //RegSetValueExA-с помощью этой функции передаем и записываем новые значения реестра
                // (hKey, -передаем в функцию значение ключа
                //"ProfilesDirectory", -имя значения
                //0, 
                //REG_SZ, тип значения
                //(LPBYTE)newProfilePath.c_str(), -передает указатель на буфер данных, содержащий новый путь профиля
                //newProfilePath.size()); -передает размер этого буфера в функцию RegSetValueExA
                if (result == ERROR_SUCCESS) {
                    std::cout << "Название папки в реестре успешно изменено." << std::endl;
                }
                else {
                    std::cout << "Ошибка при изменении названия папки в реестре." << std::endl;
                }
            }
            else {
                std::cout << "Ошибка при получении пути к папке профилей из реестра." << std::endl;
            }
            RegCloseKey(hKey);
        }
        else {
            std::cout << "Ошибка при открытии реестра." << std::endl;
        }
    }
    else {
        std::cout << "Ошибка при изменении названия папки пользователя." << std::endl;
    }
}