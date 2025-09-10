/*
 * Visualizer.h
 *
 *  Created on: Aug 7, 2025
 *      Author: nadia
 */

#ifndef VISUALIZER_H_
#define VISUALIZER_H_


#include <omnetpp.h>

using namespace omnetpp;

class Visualizer : public cSimpleModule
{
  protected:
    virtual void initialize() override;
};


#endif /* VISUALIZER_H_ */
