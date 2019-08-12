#include "systemc.h"

SC_MODULE(first_counter) {
    sc_in_clk     clk;
    sc_in<bool>    reset;
    sc_in<bool>    enable;
    sc_out<sc_unit<4>>     counter_out;
    
    sc_unit<4>     count;
    
    void incr_count() {
        if(reset.read() == 1) {
            count = 0;
            counter_out.write(count);
        } else if(enable.read() == 1) {
            count = count + 1;
            counter_out.write(count);
        }
    }
    
    SC_CTOR(first_counter) {
        SC_METHOD(incr_count);
        sensitive << reset;
        sensitive << clock.pos();
    }
}
