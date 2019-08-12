#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cassert>
#include <map>
#include <vector>
#include <cstdint>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

struct Initiator : sc_module {
    tlm_utils::simple_initiator_socket<Initiator> socket;
    
    SC_CTOR(Initiator) : socket("socket") {
        SC_THREAD(tread_process);        
        socket.register_nb_transport_bw(this, &Initiator::Initiator_Memory_nb_transport_bw);
        //socket->register_nb_transport_fw(this, &Initiator::Initiator_Memory_nb_transport_bw);
    }
    
    void tread_process() {
        tlm::tlm_generic_payload * trans = new tlm::tlm_generic_payload;
        tlm::tlm_phase phase;
        sc_time delay  = sc_core::sc_time(10, sc_core::SC_NS);
        
        for(int i=0; i<32; i=i+4) {
            tlm::tlm_command cmd = static_cast<tlm::tlm_command>(rand() % 2);
            if(cmd == tlm::TLM_WRITE_COMMAND) 
                data = 0xFF000000 | i;
            
            trans->set_command(cmd);
            trans->set_address(i);
            trans->set_data_ptr( reinterpret_cast<unsigned char*>(&data) );
            trans->set_data_length( 4 );

            phase = tlm::BEGIN_REQ;
            
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": " << "Initiator: BEGIN_REQ\n";        
            
            socket->nb_transport_fw(*trans, phase, delay);
            


            wait(delay);
        }    
    }
    
    tlm::tlm_sync_enum Initiator_Memory_nb_transport_bw(    
        tlm::tlm_generic_payload &gp
         ,tlm::tlm_phase          &phase
         ,sc_core::sc_time        &delay_time
     ){
        if(phase == tlm::END_REQ) {
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": "  << "Initiator: END_REQ \n";
        } else if(phase == tlm::BEGIN_RESP) {
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": " << "Initiator: BEGIN_RESP \n";
            
            tlm::tlm_phase bw_phase;
            bw_phase = tlm::END_RESP;
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": " << "Initiator: END_RESP\n";            
            sc_core::sc_time delay_new  = sc_core::sc_time(10, sc_core::SC_NS);
            socket->nb_transport_fw(gp, bw_phase, delay_new); // return credit            
        }
     }

    int data;
};

struct Memory : sc_module {
    tlm_utils::simple_target_socket<Memory> socket;
    
    enum { SIZE = 256 };
    
    SC_CTOR(Memory) : socket("socket") {
        //socket.register_b_transport(this, &Memory::b_transport);
        socket.register_nb_transport_fw(this, &Memory::Initiator_Memory_nb_transport_fw);
    }
    
    
    tlm::tlm_sync_enum Initiator_Memory_nb_transport_fw(     
        tlm::tlm_generic_payload& trans, 
        tlm::tlm_phase& phase, 
        sc_time& delay 
    ){
        if(phase == tlm::BEGIN_REQ) {
            tlm::tlm_phase bw_phase;
            bw_phase = tlm::END_REQ;
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": " << "Memory: BEGIN_REQ\n";            
            sc_core::sc_time delay_1  = sc_core::sc_time(10, sc_core::SC_NS);
            socket->nb_transport_bw(trans, bw_phase, delay_1); // return credit    
            
            tlm::tlm_phase bw_phase_2;
            bw_phase_2 = tlm::BEGIN_RESP;
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": " << "Memory: BEGIN_RESP\n";            
            sc_core::sc_time delay_2  = sc_core::sc_time(10, sc_core::SC_NS);
            socket->nb_transport_bw(trans, bw_phase_2, delay_2); // return credit                        
        } else if(phase == tlm::END_RESP) {
            std::cout << "@" << std::setfill('0') << std::setw(8) << sc_core::sc_time_stamp().value() << ": " << "Memory: END_RESP\n";
        }
    }

    
    int mem[SIZE];
};


SC_MODULE(Top)
{
    Initiator *initiator;
    Memory    *memory;

    SC_CTOR(Top)
    {
        // Instantiate components
        initiator = new Initiator("initiator");
        memory    = new Memory   ("memory");

        // One initiator is bound directly to one target with no intervening bus

        // Bind initiator socket to target socket
        initiator->socket.bind( memory->socket );
    }
};


int sc_main(int argc, char* argv[])
{
    Top top("top");
    sc_start();
    return 0;
}

