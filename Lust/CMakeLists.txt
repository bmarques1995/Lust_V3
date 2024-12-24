# CMakeList.txt: projeto de CMake para Lust, incluir origem e definir
# lógica específica a um projeto aqui.
#

# Adicione a origem ao executável deste projeto.
add_executable (Lust "Lust.cpp" "Lust.hpp" "entry.cpp")

set_property(TARGET Lust PROPERTY CXX_STANDARD 20)

# TODO: Adicione testes e instale destinos, se necessário.
