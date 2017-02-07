
test_accounts : process_accounts
	./process_accounts

process_accounts : process_accounts.cpp
	g++ -Wall -O3 -std=c++11 process_accounts.cpp -o process_accounts

process_tables : process_tables.cpp
	g++ -Wall -O3 -std=c++11 -fopenmp -lgomp process_tables.cpp -o process_tables

test : process_tables
	./process_tables

.DEFAULT_GOAL := process_tables
