## Come compilare ed eseguire il passo

1. Clonare la repository ed entrare nella cartella:
   ```bash
   git clone <link-repo>
   cd <cartella-progetto>

mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make

opt -load-pass-plugin=./build/LocalOpts.so -passes="pass1,pass2..." -disable-output test.ll

