all:
	g++ -std=c++11 -pthread main.cpp -o coherence utils/Processor.cpp utils/Bus.cpp utils/Cache.cpp