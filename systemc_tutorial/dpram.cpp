#include "systemc.h"

#define DATA_WIDTH 	8	
#define ADDR_WIDTH	8
#define RAM_DEPTH	1<<ADDR_WIDTH

SC_MODULE dpram {
	sc_in <sc_unit<ADDR_WIDTH>>	address_0;
	sc_in <bool> cs_0;
	sc_in <bool> we_0;
	sc_in <bool> oe_0;
	sc_in <sc_unit<ADDR_WIDTH>>	address_1;
	sc_in <bool> cs_1;
	sc_in <bool> we_1;
	sc_in <bool> oe_1;
	
	sc_inout_rv <DATA_WIDTH> data_0;
	sc_inout_rv <DATA_WIDTH> data_1;
	
	sc_unit <DATA_WIDTH> mem [RAM_DEPTH];
	
	void read_0();
	void read_1();
	void write_0();
	void write_0();
	
	SC_CTOR(dpram) {
		SC_METHOD(read_0);
			sensitive << address_0 << cs_0 << we_0 << oe_0;
		SC_METHOD(read_1);
			sensitive << address_1 << cs_1 << we_1 << oe_1;
		SC_METHOD(write_0);
			sensitive << address_0 << cs_0 << we_0 << data_0;						
		SC_METHOD(write_1);
			sensitive << address_0 << cs_0 << we_0 << data_1;			
	}
}


void dpram::read_0() {
	if( cs_0.read() && oe_0.read() && !we_0.read() ) {
		data_0 = data_0[address_0.read()] ;
	}
}

void dpram::write_0() {
	if( cs_0.read() && we_0.read() ) {
		data_0[address_0.read()] = data_0.read();
	}
}
