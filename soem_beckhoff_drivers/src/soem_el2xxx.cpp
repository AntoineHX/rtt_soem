/***************************************************************************
  tag: Ruben Smits  Tue Nov 16 09:31:20 CET 2010  soem_el2xxx.cpp

                        soem_el2xxx.cpp -  description
                           -------------------
    begin                : Tue November 16 2010
    copyright            : (C) 2010 Ruben Smits
    email                : first.last@mech.kuleuven.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "soem_el2xxx.h"
#include <soem_master/soem_driver_factory.h>

namespace soem_beckhoff_drivers{
  
  SoemEL2xxx::SoemEL2xxx(ec_slavet* mem_loc): 
    soem_master::SoemDriver(mem_loc),
    m_port("bits")
  {
    m_size=mem_loc->Obits;
    m_service->doc(std::string("Services for Beckhoff ")+std::string(m_datap->name)+std::string(" Dig. Output module"));
    m_service->addOperation("switchOn",&SoemEL2xxx::switchOn,this,RTT::OwnThread).doc("Switch bit i on").arg("i","bit nr");
    m_service->addOperation("switchOff",&SoemEL2xxx::switchOff,this,RTT::OwnThread).doc("Switch bit i off").arg("i","bit nr");
    m_service->addOperation("setBit",&SoemEL2xxx::setBit,this,RTT::OwnThread).doc("Set value of bit i to val").arg("i","bit nr").arg("val","new value for bit");
    m_service->addOperation("checkBit",&SoemEL2xxx::checkBit,this,RTT::OwnThread).doc("Check value of bit i").arg("i","bit nr");
    m_service->addConstant("size",m_size);
    m_service->addPort(m_port).doc("DigitalMsg containing the desired values of _all_ bits");
    m_msg.values.resize(m_size);

    for(unsigned int i=0;i<m_size;i++)
        setBit(i,false);
  }

  void SoemEL2xxx::update(){
	  if(m_port.connected()){
		  if(m_port.read(m_msg)==RTT::NewData){
			  if(m_msg.values.size()<m_size){
				  for(unsigned int i=0;i<m_size;i++)
					  setBit(i,m_msg.values[i]);
			  }
		  }
	  }
  }
  
  void SoemEL2xxx::setBit(unsigned int bit,bool value){
    if(bit<m_size){
    	if(value)
    		((out_el2xxxt*)(m_datap->outputs))->outbits|=(1<<(m_datap->Ostartbit+bit));
    	else
    		((out_el2xxxt*)(m_datap->outputs))->outbits&=~(1<<(m_datap->Ostartbit+bit));
    }
  }
  
  void SoemEL2xxx::switchOn(unsigned int n){
    this->setBit(n,true);
  }
  
  void SoemEL2xxx::switchOff(unsigned int n){
    this->setBit(n,false);
  }
  /*
    void SoemEL2xxx::setSequence(unsigned int start_bit,unsigned int stop_bit,unsigned int value){
    if(start_bit<size_&&stop_bit<size_){
    bits_=((out_el2xxxt*)(datap_->outputs))->outbits;
    std::bitset<8> in_bits(value);
    for(unsigned int i=start_bit;i<=stop_bit;i++)
    bits_.set(i+datap_->Ostartbit,in_bits[i]);
    ((out_el2xxxt*)(datap_->outputs))->outbits=bits_.to_ulong();
    }
    }
  */
  
  bool SoemEL2xxx::checkBit(unsigned int bit)const{
	  if(bit<m_size)
		  return ((((out_el2xxxt*)(m_datap->outputs))->outbits)&(1<<(m_datap->Ostartbit+bit)));
	  else
		  return false;
  }

  /*
  unsigned int SoemEL2xxx::checkSequence(unsigned int start_bit,unsigned int stop_bit)const{
    if(start_bit<size_&&stop_bit<size_){
      bits_=((out_el2xxxt*)(datap_->outputs))->outbits;
      std::bitset<8> out_bits;
      unsigned int j=0;
      for(unsigned int i=start_bit;i<=stop_bit;i++)
	out_bits.set(j,bits_[datap_->Ostartbit+i]);
      return out_bits.to_ulong();
    }
  }
  */
  
  namespace {
    soem_master::SoemDriver* createSoemEL2xxx(ec_slavet* mem_loc){
      return new SoemEL2xxx(mem_loc);
    }
    const bool registered0 = soem_master::SoemDriverFactory::Instance().registerDriver("EL2002",createSoemEL2xxx);
    const bool registered1 = soem_master::SoemDriverFactory::Instance().registerDriver("EL2004",createSoemEL2xxx);
    const bool registered2 = soem_master::SoemDriverFactory::Instance().registerDriver("EL2008",createSoemEL2xxx);
    const bool registered3 = soem_master::SoemDriverFactory::Instance().registerDriver("EL2124",createSoemEL2xxx);
  }
}
