
Per eseguire il pass:
```
opt -load-pass-plugin ./libLoopPass.so -passes=loop-pass ../test/LICM.ll -So ../test/LICM-opt.ll
```