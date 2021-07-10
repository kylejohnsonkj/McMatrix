FLASCHEN_TASCHEN_API_DIR=../flaschen-taschen/api

CXXFLAGS=-Wall -O3 -I$(FLASCHEN_TASCHEN_API_DIR)/include -I.
LDFLAGS=-L$(FLASCHEN_TASCHEN_API_DIR)/lib -lftclient
FTLIB=$(FLASCHEN_TASCHEN_API_DIR)/lib/libftclient.a

all : mcmatrix

mcmatrix: mcmatrix.cc

% : %.cc $(FTLIB)
	$(CXX) -std=c++11 $(CXXFLAGS) -o $@ $< $(LDFLAGS)

$(FTLIB) :
	make -C $(FLASCHEN_TASCHEN_API_DIR)/lib

run:
	make && ./mcmatrix localhost

clean:
	rm -f mcmatrix
