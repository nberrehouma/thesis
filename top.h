/*
 * top.h
 *
 *  Created on: 4 juin 2020
 *      Author: nabil
 */

#ifndef TOP_H_
#define TOP_H_

#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "initiator.h"
#include "my_mem.h"

SC_MODULE(top)
{
	tlm_memory    *memory;
	tlm_initiator *initiator;


	 SC_CTOR(top)
	   {
	     cout<<"top module  initialization"<< endl;
		 initiator = new tlm_initiator("initiator");
	     memory    = new  tlm_memory("memory");
         initiator->socket.bind( memory->socket );


};
};



#endif /* TOP_H_ */
