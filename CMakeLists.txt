cmake_minimum_required(VERSION 3.10)

project(cpu_scheduler_simulator C)

# Aggiungi la cartella "include" al percorso di inclusione
include_directories(include)

# Aggiungi tutti i file sorgente presenti nella cartella "src"
file(GLOB SOURCES src/*.c)

# Genera l'eseguibile
add_executable(main ${SOURCES} bin/carousel.c)
