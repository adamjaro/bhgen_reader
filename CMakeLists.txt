
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(convert_sr)

#BOOST
find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#HepMC3
set(HepMC3_DIR "" CACHE PATH "HepMC3 location")
find_package(HepMC3 REQUIRED)
include_directories(${HEPMC3_INCLUDE_DIR})

#ROOT
find_package(ROOT REQUIRED COMPONENTS Core EG Hist Physics RIO Tree MathCore)
include(${ROOT_USE_FILE})

#binary executable
add_executable(bhgen_reader bhgen_reader.cxx)
target_link_libraries(bhgen_reader ${Boost_LIBRARIES} ${HEPMC3_LIB} ${ROOT_LIBRARIES})

