/*
 * multiton.cpp
 *
 *  Created on: 07.02.2022
 *      Author: franz
 */

#include "multiton.h"


AbstractClientBase::~AbstractClientBase() = default;

AbstractClientBase::AbstractClientBase(int ai_multitonInst) : mi_multitonInst(ai_multitonInst){}

int AbstractClientBase::getMultitonInst() const { return mi_multitonInst;}

void AbstractClientBase::setMultitonInst(int ai_instance) { mi_multitonInst = ai_instance;}

const AbstractClientBase &AbstractClientBase::operator=(const AbstractClientBase &acrc_src)
        {
    mi_multitonInst = acrc_src.mi_multitonInst;
    return *this;
        }

AbstractClientBase::AbstractClientBase( const AbstractClientBase& acrc_src ) = default;

ClientBase::ClientBase(int ai_multitonInst) : AbstractClientBase(ai_multitonInst){}

ClientBase::~ClientBase() = default;  //this makes the Class non-abstract