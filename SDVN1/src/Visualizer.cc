/*
 * Visualizer.cc
 *
 *  Created on: Aug 7, 2025
 *      Author: nadia
 */



#include "Visualizer.h"

Define_Module(Visualizer);


void Visualizer::initialize()
{
    // 1. Define the RSU coordinates exactly as they are in your .ini file
    const std::vector<cFigure::Point> rsuPositions1 = {
        {2246.59, 2038.29},
        {2280.59, 860.29},
        {1755.54, 1241.46}
    };
    const std::vector<cFigure::Point> rsuPositions2 = {
            {1062.01, 1064.18},
            {1721.38, 1681.26},
            {1162.01, 1285.53}
        };
    const std::vector<cFigure::Point> rsuPositions3 = {
            {1349.89, 1537.46},
            {2016.01, 1232.92},
            {1332.81, 1183.06}
        };
    const std::vector<cFigure::Point> rsuPositions4 = {
            {824.68, 939.67},
            {730.74, 2038.29},
            {581.29,969.56}
            };
    const double range = 300.0; // 300m radius

    // 2. Get the main simulation canvas
    cCanvas* canvas = getSystemModule()->getCanvas();

    // 3. Loop through each coordinate and draw a circle
    for (size_t i = 0; i < 12; ++i) {
        cFigure::Point pos = rsuPositions1[i%3];
        if (i>=3 && i<6){
            pos = rsuPositions2[i%3];
        }
        if (i>=6 && i<9){
            pos = rsuPositions3[i%3];
        }
        if (i>=9){
                    pos = rsuPositions4[i%3];
                }
        auto circle = new cOvalFigure(("DebugRsuCircle_" + std::to_string(i)).c_str());
        circle->setBounds(cFigure::Rectangle(pos.x - range, pos.y - range, range * 2, range * 2));

        // Set visual style
        circle->setFilled(true);
        circle->setFillColor(cFigure::BLUE);
        if (i>=3) circle->setFillColor(cFigure::RED);
        if (i>=6) circle->setFillColor(cFigure::YELLOW);
        if (i>=9) circle->setFillColor(cFigure::GREEN);
        circle->setFillOpacity(0.2);
        circle->setLineColor(cFigure::BLACK);

        canvas->addFigure(circle);

        auto centerDot = new cOvalFigure(("DebugRsuDot_" + std::to_string(i)).c_str());
        double dotRadius = 5.0; // A small 5m radius for the dot
        centerDot->setBounds(cFigure::Rectangle(pos.x - dotRadius, pos.y - dotRadius, dotRadius * 2, dotRadius * 2));
        centerDot->setFilled(true);
        centerDot->setFillColor(cFigure::RED); // Make the dot a solid, visible color
        centerDot->setLineColor(cFigure::BLACK);
        canvas->addFigure(centerDot);
    }
}
