/*
 * process.h
 *
 *  Created on: 4 juin 2020
 *      Author: nabil
 */

#ifndef MY_MEM_H_
#define MY_MEM_H_
#define SC_INCLUDE_DYNAMIC_PROCESSES
#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
using namespace tlm;
using namespace tlm_utils;
SC_MODULE(tlm_memory)
{

	 simple_target_socket<tlm_memory> socket;

  SC_CTOR(tlm_memory): socket("socket"),LATENCY(10, SC_NS)
  {
	 //S cout<<"memory module  initialization"<< endl;
	  // Register callback for incoming b_transport interface method call
	      socket.register_b_transport(this, &tlm_memory::b_transport);
	      socket.register_get_direct_mem_ptr(this, &tlm_memory::get_direct_mem_ptr);
	      socket.register_transport_dbg(this, &tlm_memory::transport_dbg);

	      // Initialize memory with random data
	      for (int i = 0; i < SIZE; i++)
	        mem[i] = 0xAA000000 | (rand() % 256);

	      SC_THREAD(invalidation_process);
  }
  // TLM-2 blocking transport method
    virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
    {
      tlm::tlm_command cmd = trans.get_command();
      sc_dt::uint64    adr = trans.get_address() / 4;
      unsigned char*   ptr = trans.get_data_ptr();
      unsigned int     len = trans.get_data_length();
      unsigned char*   byt = trans.get_byte_enable_ptr();
      unsigned int     wid = trans.get_streaming_width();

      // Obliged to check address range and check for unsupported features,
      //   i.e. byte enables, streaming, and bursts
      // Can ignore DMI hint and extensions
      // Using the SystemC report handler is an acceptable way of signalling an error

      //if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > 4 || wid < len)
        //SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

      if (adr >= sc_dt::uint64(SIZE)) {
          trans.set_response_status( TLM_ADDRESS_ERROR_RESPONSE );
          return;
        }

      if (byt != 0) {
          trans.set_response_status( TLM_BYTE_ENABLE_ERROR_RESPONSE );
          return;
        }

      if (len > 4 || wid < len) {
          trans.set_response_status( TLM_BURST_ERROR_RESPONSE );
          return;
        }

      // Obliged to implement read and write commands
      if ( cmd == tlm::TLM_READ_COMMAND )
        memcpy(ptr, &mem[adr], len);
      else if ( cmd == tlm::TLM_WRITE_COMMAND )
        memcpy(&mem[adr], ptr, len);

      // Illustrates that b_transport may block
          wait(delay);
      // Reset timing annotation after waiting
       delay = SC_ZERO_TIME;
      // Obliged to set response status to indicate successful completion
      trans.set_response_status( tlm::TLM_OK_RESPONSE );
      trans.set_dmi_allowed(true);
    }

    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                    tlm::tlm_dmi& dmi_data)
    {
      dmi_data.allow_read_write();
      dmi_data.set_dmi_ptr( reinterpret_cast<unsigned char*>( &mem[0] ) );
      dmi_data.set_start_address( 0 );
      dmi_data.set_end_address( SIZE*4-1 );
      dmi_data.set_read_latency( LATENCY );
      dmi_data.set_write_latency( LATENCY );

        return true;
    }

      // *********************************************
      // TLM-2 debug transport method
      // *********************************************

      virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans)
      {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    adr = trans.get_address() / 4;
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();

        // Calculate the number of bytes to be actually copied
        unsigned int num_bytes = (len < (SIZE - adr) * 4) ? len : (SIZE - adr) * 4;

        if ( cmd == tlm::TLM_READ_COMMAND )
          memcpy(ptr, &mem[adr], num_bytes);
        else if ( cmd == tlm::TLM_WRITE_COMMAND )
          memcpy(&mem[adr], ptr, num_bytes);

        return num_bytes;
      }

      void invalidation_process()
        {
          // Invalidate DMI pointers periodically
          for (int i = 0; i < 4; i++)
          {
            wait(LATENCY*8);
            socket->invalidate_direct_mem_ptr(0, SIZE-1);
          }
        }

      enum { SIZE = 256 };
      	int mem[SIZE];
      	const sc_time LATENCY;

};



#endif /* MY_MEM_H_ */
