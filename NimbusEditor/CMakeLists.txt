cmake_minimum_required(VERSION 3.16)
project(NimbusEditor VERSION 1.0 LANGUAGES CXX)

# C++17 standardını zorunlu kıl (std::filesystem için gerekli)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Otomatik Qt işlemleri için ayarları yap (MOC, UIC, RCC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Gerekli Qt6 paketlerini bul
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Çalıştırılabilir dosyayı ve C++ kaynak kodlarını tanımla
add_executable(NimbusEditor
  main.cpp
  MainWindow.cpp
)

# Derleyiciye Qt'nin başlık dosyalarının (.h) nerede olduğunu açıkça söyle
target_include_directories(NimbusEditor PRIVATE ${Qt6_Widgets_INCLUDE_DIRS})

# Gerekli Qt modüllerini projemize bağla (link et)
target_link_libraries(NimbusEditor PRIVATE Qt6::Widgets)