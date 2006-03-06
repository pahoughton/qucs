/***************************************************************************
                                switch.cpp
                               ------------
    begin                : Sat Feb 25 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "switch.h"


Switch::Switch()
{
  Description = QObject::tr("switch (time controlled)");

  Lines.append(new Line(-30,  0,-15,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line( 17,  0, 30,  0,QPen(QPen::darkBlue,2)));
  Lines.append(new Line(-15,  0, 15,-15,QPen(QPen::darkBlue,2)));
  Arcs.append(new Arc( 12, -3, 6, 6, 0, 16*360,QPen(QPen::darkBlue,2)));
  Ellips.append(new Area(-18, -3, 6, 6, QPen(QPen::darkBlue,2),
                         QBrush(Qt::darkBlue, Qt::SolidPattern)));

  Ports.append(new Port(-30,  0));
  Ports.append(new Port( 30,  0));

  x1 = -30; y1 = -17;
  x2 =  30; y2 =   7;

  tx = x1+4;
  ty = y2+4;
  Model = "Switch";
  Name  = "S";

  Props.append(new Property("init", "off", false,
		QObject::tr("initial state")+" [on, off]"));
  Props.append(new Property("time", "1 ms", false,
		QObject::tr("time when state changes (comma separated list possible)")));
  Props.append(new Property("Ron", "0", false,
		QObject::tr("resistance of \"on\" state in ohms")));
  Props.append(new Property("Roff", "1e12", false,
		QObject::tr("resistance of \"off\" state in ohms")));
  Props.append(new Property("Temp", "26.85", false,
		QObject::tr("simulation temperature in degree Celsius")));
}

Switch::~Switch()
{
}

Component* Switch::newOne()
{
  return new Switch();
}

Element* Switch::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Switch");
  BitmapFile = "switch";

  if(getNewOne)  return new Switch();
  return 0;
}
