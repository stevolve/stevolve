emcc ../STEVolve/portable/CellBase.cpp \
     ../STEVolve/portable/CellNeural.cpp \
     ../STEVolve/portable/CellProgram.cpp \
     ../STEVolve/portable/WorldBase.cpp \
     ../STEVolve/portable/WorldToroidal.cpp \
     ../STEVolve/portable/Settings.cpp \
     STEvolve.cpp \
     --emrun -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s LLD_REPORT_UNDEFINED -s ASYNCIFY -s ASYNCIFY_STACK_SIZE=1048576 \
     -g -o STEvolve.html