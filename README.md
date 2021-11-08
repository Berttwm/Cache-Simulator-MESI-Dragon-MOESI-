# cs4223_simulator

Compile the main code first
```
g++ -pthread main.cpp -o coherence utils/Processor.cpp utils/Bus.cpp utils/Cache.cpp
```

Try one random input
```
./coherence MESI bodytrack 1024 1 16
```