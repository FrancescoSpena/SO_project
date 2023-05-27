cmake_minimum_required(VERSION 3.10)

project(cpu_scheduler_simulator C)

# Ottieni il percorso assoluto della cartella di build
get_filename_component(BUILD_DIR ${CMAKE_BINARY_DIR} DIRECTORY)

# Imposta la directory di output per gli eseguibili
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR}/bin)

# Imposta la directory di output per le librerie
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}/lib)

# Imposta la directory di output per gli oggetti
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR}/obj)

# Aggiungi la cartella "include" al percorso di inclusione
include_directories(include)

# Aggiungi tutti i file sorgente presenti nella cartella "src"
file(GLOB SOURCES src/*.c)

# Genera l'eseguibile
add_executable(main ${SOURCES})
