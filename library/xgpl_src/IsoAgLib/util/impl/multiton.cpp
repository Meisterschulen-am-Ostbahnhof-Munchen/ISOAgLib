/*
 * multiton.cpp
 *
 *  Created on: 07.02.2022
 *      Author: franz
 */

#include "multiton.h"


AbstractClientBase::~AbstractClientBase() = default;

AbstractClientBase::AbstractClientBase(multiton ai_multitonInst) : mi_multitonInst(ai_multitonInst){}

multiton AbstractClientBase::getMultitonInst() const { return mi_multitonInst;}

void AbstractClientBase::setMultitonInst(multiton ai_instance) { mi_multitonInst = ai_instance;}

const AbstractClientBase &AbstractClientBase::operator=(const AbstractClientBase &acrc_src)
        {
    mi_multitonInst = acrc_src.mi_multitonInst;
    return *this;
        }

AbstractClientBase::AbstractClientBase( const AbstractClientBase& acrc_src ) = default;

ClientBase::ClientBase(multiton ai_multitonInst) : AbstractClientBase(ai_multitonInst){}

ClientBase::~ClientBase() = default;  //this makes the Class non-abstract
