# cs4223_simulator

Compile the main code first
```
g++ -pthread main.cpp -o coherence utils/Processor.cpp utils/Bus.cpp utils/Cache.cpp
```

Try some default inputs
```
./coherence MESI bodytrack 4096 2 32
./coherence MOESI bodytrack 4096 2 32
./coherence Dragon bodytrack 4096 2 32

./coherence MESI blackscholes 4096 2 32
./coherence MESI bodytrack 4096 2 32
./coherence MESI fluidanimate 4096 2 32

./coherence MOESI blackscholes 4096 2 32
./coherence MOESI bodytrack 4096 2 32
./coherence MOESI fluidanimate 4096 2 32

./coherence Dragon blackscholes 4096 2 32
./coherence Dragon bodytrack 4096 2 32
./coherence Dragon fluidanimate 4096 2 32

```