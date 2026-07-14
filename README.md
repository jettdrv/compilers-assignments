# Compilers Assignments
Repository contenente gli assignments del corso di Linguaggi e Compilatori - Parte Middle-End.

## Contenuti 
* Primo assignment: 
  * identità algebrighe
  * strength reduce avanzato
  * constant propagation
* Secondo assignment: analisi dataflow:
  * Very Busy Expressions
  * Dominator Analysis
  * Constant Propagation
* Terzo assignment: LICM
* Quarto assignment: Loop Fusion

## Guida alla compilazione
'''
export LLVM_DIR=<installation/dir/of/llvm>/bin
mkdir build 
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR <source/dir/test/pass>
make
'''
