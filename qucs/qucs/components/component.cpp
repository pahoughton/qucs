/***************************************************************************
                          component.cpp  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "components.h"

#include <qpoint.h>
#include <qpainter.h>
#include <qstring.h>
#include <qpen.h>
#include <qmessagebox.h>



// ***********************************************************************************
// **********                                                               **********
// **********                      class "Component"                        **********
// **********                                                               **********
// ***********************************************************************************
Component::Component()
{
  Type = isComponent;

  mirroredX = false;
  rotated = 0;
  isSelected = false;
  isActive   = true;

  cx = 0;
  cy = 0;

  Arcs.setAutoDelete(true);
  Lines.setAutoDelete(true);
  Ports.setAutoDelete(true);
  Texts.setAutoDelete(true);
  Props.setAutoDelete(true);
}

Component::~Component()
{
}

Component* Component::newOne()
{
  return new Component();
}

// -------------------------------------------------------
void Component::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;
}

// -------------------------------------------------------
// Boundings including the component text.
void Component::entireBounds(int& _x1, int& _y1, int& _x2, int& _y2)
{
  _x1 = x1+cx;
  _y1 = y1+cy;
  _x2 = x2+cx;
  _y2 = y2+cy;

  // text boundings
  if(tx < x1) _x1 = tx+cx;
  if(ty < y1) _y1 = ty+cy;

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",12, QFont::Light));
  QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,Name);      // get width of text
  if((tx+r.width()) > x2) _x2 = tx+r.width()+cx;
  if((ty+r.height()) > y2) _y2 = ty+r.height()+cy;

  int dy=12; // due to 'Name' text
  for(Property *pp = Props.first(); pp != 0; pp = Props.next())
    if(pp->display) {
      r = p.boundingRect(0,0,0,0,Qt::AlignAuto,pp->Name+"="+pp->Value);      // get width of text
      if((tx+r.width()) > x2) _x2 = tx+r.width()+cx;
      dy += 12;
    }
  if((ty+dy) > y2) _y2 = ty+dy+cy;
}

// -------------------------------------------------------
void Component::setCenter(int x, int y, bool relative)
{
  if(relative) { cx += x;  cy += y; }
  else { cx = x;  cy = y; }
}

// -------------------------------------------------------
bool Component::getSelected(int x_, int y_)
{
  if(x_ >= x1+cx) if(x_ <= x2+cx) if(y_ >= y1+cy) if(y_ <= y2+cy)
    return true;

  return false;
}

// -------------------------------------------------------
void Component::paint(QPainter *p)
{
  
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {     // paint all lines
    p->setPen(p1->style);
    p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // paint all arcs
    p->setPen(p3->style);
    p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
  }

  QFont tmp;
  if(Sign.at(0) == '.') {   // is simulation component (dc, ac, s parameter, ...)
    tmp = p->font();
    p->setFont(QFont("Helvetica",16, QFont::DemiBold));
    p->drawText(cx+x1+8, cy+y1+8, x2-x1, y2-y1, Qt::WordBreak, Description);
    p->setFont(tmp);
  }

  p->setPen(QPen(QPen::black,1));
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next()) {     // write all text
    tmp = p->font();
    p->setFont(QFont("Helvetica",10, QFont::Light));
    p->drawText(cx+pt->x, cy+pt->y, pt->s);
    p->setFont(tmp);
  }

  int y=12;
  p->drawText(cx+tx, cy+ty, 0, 0, Qt::DontClip, Name);
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next()) {    // write all properties
    if(p4->display) p->drawText(cx+tx, cy+ty+y, 0, 0, Qt::DontClip, p4->Name+"="+p4->Value);
    y += 12;
  }

  if(!isActive) {
    p->setPen(QPen(QPen::red,1));
    p->drawRect(cx+x1, cy+y1, x2-x1, y2-y1);
    p->drawLine(cx+x1, cy+y1, cx+x2, cy+y2);
    p->drawLine(cx+x1, cy+y2, cx+x2, cy+y1);
  }

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(cx+x1, cy+y1, x2-x1, y2-y1);
  }
}

// -------------------------------------------------------
// Paints the component when moved with the mouse.
void Component::paintScheme(QPainter *p) //, int x, int y) //const QPoint& pos)
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next())     // paint all lines
    p->drawLine(cx+p1->x1, cy+p1->y1, cx+p1->x2, cy+p1->y2);

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())     // paint all ports
    p->drawEllipse(cx+p2->x-4, cy+p2->y-4, 8, 8);
  
  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next())     // paint all arcs
    p->drawArc(cx+p3->x, cy+p3->y, p3->w, p3->h, p3->angle, p3->arclen);
}

// -------------------------------------------------------
// Rotates the component 90� counter-clockwise around its center
void Component::rotate()
{
  int tmp;
  
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {     // rotate all lines
    tmp = -p1->x1;
    p1->x1 = p1->y1;
    p1->y1 = tmp;
    tmp = -p1->x2;
    p1->x2 = p1->y2;
    p1->y2 = tmp;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next()) {     // rotate all ports
    tmp = -p2->x;
    p2->x = p2->y;
    p2->y = tmp;
  }

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // rotate all arcs
    tmp = -p3->x;
    p3->x = p3->y;
    p3->y = tmp - p3->w +1; // +1 is beauty correction
    tmp = p3->w;
    p3->w = p3->h;
    p3->h = tmp;
    p3->angle += 16*90;
    if(p3->angle >= 16*360) p3->angle -= 16*360;;
  }

  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next()) {     // rotate all text
    tmp = -pt->x; // - r.width();
    pt->x = pt->y - 8;
    pt->y = tmp;
  }

  tmp = -x1;   // rotate boundings
  x1  = y1; y1 = -x2;
  x2  = y2; y2 = tmp;
  
  tmp = tx;    // rotate text position
  tx  = ty;
  ty  = tmp;

  rotated++;  // keep track of what's done
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the x-axis.
void Component::mirrorX()
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {  // mirror all lines
    p1->y1 = -p1->y1;
    p1->y2 = -p1->y2;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())  // mirror all ports
    p2->y = -p2->y;

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // mirror all arcs
    p3->y = -p3->y - p3->h + 1; // +1 is beauty correction
    if(p3->angle > 16*180) p3->angle -= 16*360;
    p3->angle  = -p3->angle;  // mirror
    p3->angle -= p3->arclen;  // go back to end of arc
    if(p3->angle < 0) p3->angle += 16*360;  // angle has to be > 0
  }

  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next())     // mirror all text
    pt->y = -pt->y + 10;

  int tmp = y1;
  y1  = -y2; y2 = -tmp;   // mirror boundings
  if(tx<0) ty = y2+4;   // mirror text position
  else ty = y1+4;

  mirroredX = !mirroredX;   // keep track of what's done
  rotated += rotated << 1;
  rotated &= 3;
}

// -------------------------------------------------------
// Mirrors the component about the y-axis.
void Component::mirrorY()
{
  for(Line *p1 = Lines.first(); p1 != 0; p1 = Lines.next()) {  // mirror all lines
    p1->x1 = -p1->x1;
    p1->x2 = -p1->x2;
  }

  for(Port *p2 = Ports.first(); p2 != 0; p2 = Ports.next())  // mirror all ports
    p2->x = -p2->x;

  for(Arc *p3 = Arcs.first(); p3 != 0; p3 = Arcs.next()) {     // mirror all arcs
    p3->x = -p3->x - p3->w + 1; // +1 is beauty correction
    p3->angle = 16*180 - p3->angle - p3->arclen;  // mirror
    if(p3->angle < 0) p3->angle += 16*360;   // angle has to be > 0
  }

  QWidget w;
  QPainter p(&w);
  p.setFont(QFont("Helvetica",10, QFont::Light));
  for(Text *pt = Texts.first(); pt != 0; pt = Texts.next()) {     // mirror all text
    QRect r = p.boundingRect(0,0,0,0,Qt::AlignAuto,pt->s);    // get width of text
    pt->x = -pt->x - r.width();
  }

  int tmp = x1;
  x1  = -x2; x2 = -tmp;   // mirror boundings
  if(ty<0) tx = x2+4;   // mirror text position
  else tx = x1+4;

  mirroredX = !mirroredX;   // keep track of what's done
  rotated += rotated << 1;
  rotated += 2;
  rotated &= 3;
}

// -------------------------------------------------------
QString Component::NetList()
{
  if(Model.isEmpty()) return QString("");   // dummy elements (e.g. ground)
  if(!isActive) return QString("");         // should it be simulated ?
  QString s = Model+":"+Name;
  
  
  for(Port *p1 = Ports.first(); p1 != 0; p1 = Ports.next())     // work on all ports
    s += " "+p1->Connection->Name;    // node names

  for(Property *p2 = Props.first(); p2 != 0; p2 = Props.next())    // work on all properties
    s += " "+p2->Name+"=\""+p2->Value+"\"";

  return s;
}

// -------------------------------------------------------
QString Component::save()
{
  QString s = "   <"+Sign;
  if(Name.isEmpty()) s += " *";
  else s += " "+Name;

  if(isActive) s += " 1";
  else s += " 0";
  s += " "+QString::number(cx)+" "+QString::number(cy);
  s += " "+QString::number(tx)+" "+QString::number(ty);
  if(mirroredX) s += " 1";
  else s += " 0";
  s += " "+QString::number(rotated);
  
  for(Property *p1 = Props.first(); p1 != 0; p1 = Props.next()) {   // write all properties
    if(p1->Description.isEmpty())  s += " \""+p1->Name+"="+p1->Value+"\"";     // e.g. for equations
    else s += " \""+p1->Value+"\"";
    if(p1->display) s += " 1";
    else s += " 0";
  }

  return s+">";
}

// -------------------------------------------------------
bool Component::load(const QString& _s)
{
  bool ok;
  int  ttx, tty, tmp;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  QString n;
  Sign = s.section(' ',0,0);    // Sign

  Name = s.section(' ',1,1);    // Name
  if(Name == "*") Name = "";

  n  = s.section(' ',2,2);      // isActive
  if(n.toInt(&ok) == 1) isActive = true;
  else isActive = false;
  if(!ok) return false;

  n  = s.section(' ',3,3);    // cx
  cx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // cy
  cy = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // tx
  ttx = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);    // ty
  tty = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',7,7);    // mirroredX
  if(n.toInt(&ok) == 1) mirrorX();  // mirror component
  if(!ok) return false;

  n  = s.section(' ',8,8);    // rotated
  tmp = n.toInt(&ok);
  if(!ok) return false;
  for(int z=0; z<tmp; z++) rotate();   // rotate component

  tx = ttx; ty = tty;   // restore text position (was change by rotate/mirror)

  int z=0;
  for(Property *p1 = Props.first(); p1 != 0; p1 = Props.next()) {   // load all properties
    z++;
    n = s.section('"',z,z);    // property value
    if(n.isEmpty()) {  // not all properties have to be mentioned (backward compatible)
      if(p1->Description.isEmpty()) Props.remove();    // remove if allocated in vain
      return true;
    }
    if(p1->Description.isEmpty()) {   // unknown number of properties follows ?
      p1->Name = n.section('=',0,0);
      n = n.section('=',1,1);
      Props.append(new Property("y", "1", true));   // allocate memory for a new property (e.g. for equations)
      Props.prev();
    }
    p1->Value = n;

    z++;
    n  = s.section('"',z,z);    // display
    if(n.toInt(&ok) == 1) p1->display = true;
    else p1->display = false;
    if(!ok) return false;
  }

  return true;
}



// ********************************************************************************
// **********                                                            **********
// **********    The following function does not below to any class.     **********
// **********    It creates a component by getting the identification    **********
// **********    string used in the schematic file and for copy/paste.   **********
// **********                                                            **********
// ********************************************************************************

Component* getComponentFromName(QString& Line)
{
  Component *c = 0;

  Line = Line.stripWhiteSpace();
  if(Line.at(0) != '<') {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Format Error:\nWrong line start!"));
    return 0;
  }

  QString cstr = Line.section(' ',0,0);    // component type
  char first = Line.at(1).latin1();     // first letter of component name
  cstr.remove(0,2);    // remove leading "<" and first letter

  // to speed up the string comparision, they are ordered by the first letter of their name
  switch(first) {
  case 'R' : if(cstr.isEmpty()) c = new Resistor();
        else if(cstr == "us") c = new ResistorUS();
        break;
  case 'C' : if(cstr.isEmpty()) c = new Capacitor();
        else if(cstr == "CCS") c = new CCCS();
        else if(cstr == "CVS") c = new CCVS();
        else if(cstr == "irculator") c = new Circulator();
        break;
  case 'L' : if(cstr.isEmpty()) c = new Inductor();
        break;
  case 'G' : if(cstr == "ND") c = new Ground();
        else if(cstr == "yrator") c = new Gyrator();
        break;
  case 'I' : if(cstr == "Probe") c = new iProbe();
        else if(cstr == "dc") c = new Ampere_dc();
        else if(cstr == "noise") c = new Ampere_noise();
        else if(cstr == "solator") c = new Isolator();
        break;
  case 'V' : if(cstr == "dc") c = new Volt_dc();
        else if(cstr == "ac") c = new Volt_ac();
        else if(cstr == "CCS") c = new VCCS();
        else if(cstr == "CVS") c = new VCVS();
        else if(cstr == "noise") c = new Volt_noise();
        break;
  case 'T' : if(cstr == "r") c = new Transformer();
        else if(cstr == "LIN") c = new TLine();
        break;
  case 's' : if(cstr == "Tr") c = new symTrafo();
        break;
  case 'P' : if(cstr == "ac") c = new Source_ac();
        else if(cstr == "ort") c = new SubCirPort();
        else if(cstr == "Shift") c = new Phaseshifter();
        break;
  case 'S' : if(cstr.left(7) == "Pfile") { c = new SParamFile(cstr.mid(7).toInt()); }
        else if(cstr.left(4) == "ub") { c = new Subcircuit(cstr.mid(4).toInt()); }
        else if(cstr == "UBST") c = new Substrate();
        break;
  case 'D' : if(cstr == "CBlock") c = new dcBlock();
        else if(cstr == "CFeed") c = new dcFeed();
        else if(cstr == "iode") c = new Diode();
        break;
  case 'B' : if(cstr == "iasT") c = new BiasT();
        break;
  case 'A' : if(cstr == "ttenuator") c = new Attenuator();
        break;
  case 'M' : if(cstr == "LIN") c = new MSline();
        else if(cstr == "STEP") c = new MSstep();
        else if(cstr == "CORN") c = new MScorner();
        else if(cstr == "TEE") c = new MStee();
        else if(cstr == "CROSS") c = new MScross();
        else if(cstr == "MBEND") c = new MSmbend();
        else if(cstr == "OPEN") c = new MSopen();
        break;
  case 'E' : if(cstr == "qn") c = new Equation();
        break;
  case '.' : if(cstr == "DC") c = new DC_Sim();
        else if(cstr == "AC") c = new AC_Sim();
        else if(cstr == "TR") c = new TR_Sim();
        else if(cstr == "SP") c = new SP_Sim();
        else if(cstr == "HB") c = new HB_Sim();
        else if(cstr == "SW") c = new Param_Sweep();
        break;
  }
  if(!c) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Format Error:\nUnknown component!"));
    return 0;
  }

  if(!c->load(Line)) {
    QMessageBox::critical(0, QObject::tr("Error"),
                 QObject::tr("Format Error:\nWrong 'component' line format!"));
    delete c;
    return 0;
  }

  return c;
}
