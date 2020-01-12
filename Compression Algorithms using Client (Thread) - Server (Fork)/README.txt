Command:
g++ -std=c++11 -o c client.cpp -lpthread
g++ -std=c++11 -o s server.cpp

./s 5678
./c localhost 5678 <test*.txt 