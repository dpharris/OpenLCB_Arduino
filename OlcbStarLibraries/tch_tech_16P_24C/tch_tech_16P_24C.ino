//==============================================================
// tch_tech_16P_24C
//   A 16 input, 24 output OpenLCB node
//
//   Bob Jacobsen 2010
//      based on examples by Alex Shepherd, David Harris and Tim Hatch
//==============================================================

// next line for stand-alone compile
#include <Arduino.h>

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "logging.h"
//#include <MemoryFree.h>
// The following lines are needed because the Arduino environment 
// won't search a library directory unless the library is included 
// from the top level file (this file)
#include <EEPROM.h>
#include <can.h>
#include "NodeID.h"
#include <ButtonLed.h>


// init for serial communications
//#define         BAUD_RATE       115200
//#define         BAUD_RATE       57600
//#define         BAUD_RATE       19200


NodeID nodeid(2,1,54,4,18,4);    // This node's default ID

unsigned int datagramCallback(uint8_t *rbuf, unsigned int length, unsigned int from);
unsigned int streamRcvCallback(uint8_t *rbuf, unsigned int length);

/**
 * Get and put routines that 
 * use a test memory space.
 */

extern "C" {
const char configDefInfo[] PROGMEM = {
   60, 63, 120, 109, 108, 32, 118, 101, 114, 115, 105, 111, 110, 61, 39, 49, 46, 48, 39, 63, 62, 10, 60, 63, 120, 109, 108, 45, 115, 116, 121, 108, 101, 115, 104, 101, 101, 116, 32, 116, 121, 112, 101, 61, 39, 116, 101, 120, 116, 47, 120, 115, 108, 39, 32, 104, 114, 101, 102, 61, 39, 104, 116, 116, 112, 58, 47, 47, 111, 112, 101, 110, 108, 99, 98, 46, 111, 114, 103, 47, 116, 114, 117, 110, 107, 47, 112, 114, 111, 116, 111, 116, 121, 112, 101, 115, 47, 120, 109, 108, 47, 120, 115, 108, 116, 47, 99, 100, 105, 46, 120, 115, 108, 39, 63, 62, 10, 60, 99, 100, 105, 32, 120, 109, 108, 110, 115, 58,    // | <?xml version='1.0'?><?xml-stylesheet type='text/xsl' href='http://openlcb.org/trunk/prototypes/xml/xslt/cdi.xsl'?><cdi xmlns:|
   120, 115, 105, 61, 39, 104, 116, 116, 112, 58, 47, 47, 119, 119, 119, 46, 119, 51, 46, 111, 114, 103, 47, 50, 48, 48, 49, 47, 88, 77, 76, 83, 99, 104, 101, 109, 97, 45, 105, 110, 115, 116, 97, 110, 99, 101, 39, 32, 120, 115, 105, 58, 110, 111, 78, 97, 109, 101, 115, 112, 97, 99, 101, 83, 99, 104, 101, 109, 97, 76, 111, 99, 97, 116, 105, 111, 110, 61, 39, 104, 116, 116, 112, 58, 47, 47, 111, 112, 101, 110, 108, 99, 98, 46, 111, 114, 103, 47, 116, 114, 117, 110, 107, 47, 112, 114, 111, 116, 111, 116, 121, 112, 101, 115, 47, 120, 109, 108, 47, 115, 99, 104, 101, 109, 97, 47, 99, 100,    // | xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:noNamespaceSchemaLocation='http://openlcb.org/trunk/prototypes/xml/schema/cd|
   105, 46, 120, 115, 100, 39, 62, 10, 10, 60, 105, 100, 101, 110, 116, 105, 102, 105, 99, 97, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 60, 109, 97, 110, 117, 102, 97, 99, 116, 117, 114, 101, 114, 62, 84, 67, 72, 32, 84, 101, 99, 104, 110, 111, 108, 111, 103, 121, 60, 47, 109, 97, 110, 117, 102, 97, 99, 116, 117, 114, 101, 114, 62, 10, 32, 32, 32, 32, 60, 109, 111, 100, 101, 108, 62, 79, 112, 101, 110, 76, 67, 66, 32, 49, 54, 32, 105, 110, 32, 50, 52, 32, 111, 117, 116, 32, 80, 47, 67, 32, 110, 111, 100, 101, 60, 47, 109, 111, 100, 101, 108, 62, 10, 32, 32, 32, 32, 60,    // | i.xsd'><identification>    <manufacturer>TCH Technology</manufacturer>    <model>OpenLCB 16 in 24 out P/C node</model>    <|
   104, 97, 114, 100, 119, 97, 114, 101, 86, 101, 114, 115, 105, 111, 110, 62, 49, 46, 48, 46, 50, 60, 47, 104, 97, 114, 100, 119, 97, 114, 101, 86, 101, 114, 115, 105, 111, 110, 62, 10, 32, 32, 32, 32, 60, 115, 111, 102, 116, 119, 97, 114, 101, 86, 101, 114, 115, 105, 111, 110, 62, 48, 46, 55, 46, 54, 60, 47, 115, 111, 102, 116, 119, 97, 114, 101, 86, 101, 114, 115, 105, 111, 110, 62, 10, 60, 47, 105, 100, 101, 110, 116, 105, 102, 105, 99, 97, 116, 105, 111, 110, 62, 10, 10, 60, 115, 101, 103, 109, 101, 110, 116, 32, 111, 114, 105, 103, 105, 110, 61, 39, 48, 39, 32, 115, 112, 97, 99,    // | hardwareVersion>1.0.2</hardwareVersion>    <softwareVersion>0.7.6</softwareVersion></identification><segment origin='0' spac|
   101, 61, 39, 50, 53, 51, 39, 62, 10, 32, 32, 32, 32, 60, 103, 114, 111, 117, 112, 32, 111, 102, 102, 115, 101, 116, 61, 39, 56, 49, 54, 39, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 85, 115, 101, 114, 32, 73, 100, 101, 110, 116, 105, 102, 105, 99, 97, 116, 105, 111, 110, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 100, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 62, 76, 101, 116, 115, 32, 116, 104, 101, 32, 117, 115, 101, 114, 32, 97, 100, 100, 32, 104, 105, 115, 32, 111, 119, 110, 32, 100, 101, 115, 99, 114, 105,    // | e='253'>    <group offset='816'>        <name>User Identification</name>        <description>Lets the user add his own descri|
   112, 116, 105, 111, 110, 60, 47, 100, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115, 116, 114, 105, 110, 103, 32, 115, 105, 122, 101, 61, 39, 50, 48, 39, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 78, 111, 100, 101, 32, 78, 97, 109, 101, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 115, 116, 114, 105, 110, 103, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115, 116, 114, 105, 110, 103, 32, 115, 105, 122, 101, 61, 39, 51, 48, 39, 62, 10, 32,    // | ption</description>        <string size='20'>            <name>Node Name</name>        </string>        <string size='30'> |
   32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 78, 111, 100, 101, 32, 68, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 115, 116, 114, 105, 110, 103, 62, 10, 32, 32, 32, 32, 60, 47, 103, 114, 111, 117, 112, 62, 10, 60, 47, 115, 101, 103, 109, 101, 110, 116, 62, 10, 10, 60, 115, 101, 103, 109, 101, 110, 116, 32, 111, 114, 105, 103, 105, 110, 61, 39, 49, 48, 39, 32, 115, 112, 97, 99, 101, 61, 39, 50, 53, 51, 39, 62, 10, 32, 32, 32, 32, 60, 103, 114, 111, 117, 112,    // |            <name>Node Description</name>        </string>    </group></segment><segment origin='10' space='253'>    <group|
   32, 114, 101, 112, 108, 105, 99, 97, 116, 105, 111, 110, 61, 39, 50, 52, 39, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 79, 117, 116, 112, 117, 116, 115, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 114, 101, 112, 110, 97, 109, 101, 62, 79, 117, 116, 112, 117, 116, 60, 47, 114, 101, 112, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115, 116, 114, 105, 110, 103, 32, 115, 105, 122, 101, 61, 39, 49, 54, 39, 32, 111, 102, 102, 115, 101, 116, 61, 39, 56, 54, 48, 39, 62, 10, 32, 32, 32, 32, 32, 32,    // |  replication='24'>        <name>Outputs</name>        <repname>Output</repname>        <string size='16' offset='860'>      |
   32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 68, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 115, 116, 114, 105, 110, 103, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 101, 118, 101, 110, 116, 105, 100, 32, 111, 102, 102, 115, 101, 116, 61, 39, 45, 56, 55, 52, 39, 32, 62, 60, 33, 45, 45, 32, 102, 111, 114, 119, 97, 114, 100, 32, 49, 53, 48, 44, 32, 97, 100, 100, 32, 49, 54, 44, 32, 98, 97, 99, 107, 32, 49, 54, 54, 44, 32, 116, 104, 101, 110, 32, 102, 111, 114, 119, 97, 114,    // |       <name>Description</name>        </string>        <eventid offset='-874' ><!-- forward 150, add 16, back 166, then forwar|
   100, 32, 50, 45, 45, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 65, 99, 116, 105, 118, 97, 116, 105, 111, 110, 32, 69, 118, 101, 110, 116, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 101, 118, 101, 110, 116, 105, 100, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 101, 118, 101, 110, 116, 105, 100, 32, 111, 102, 102, 115, 101, 116, 61, 39, 50, 39, 32, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 73, 110, 97, 99, 116, 105, 118, 97, 116, 105, 111, 110,    // | d 2-->            <name>Activation Event</name>        </eventid>        <eventid offset='2' >            <name>Inactivation|
   32, 69, 118, 101, 110, 116, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 101, 118, 101, 110, 116, 105, 100, 62, 10, 32, 32, 32, 32, 60, 47, 103, 114, 111, 117, 112, 62, 10, 32, 32, 32, 32, 60, 103, 114, 111, 117, 112, 32, 114, 101, 112, 108, 105, 99, 97, 116, 105, 111, 110, 61, 39, 49, 54, 39, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 73, 110, 112, 117, 116, 115, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 114, 101, 112, 110, 97, 109, 101, 62, 73, 110, 112, 117, 116, 60, 47, 114,    // |  Event</name>        </eventid>    </group>    <group replication='16'>        <name>Inputs</name>        <repname>Input</r|
   101, 112, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 115, 116, 114, 105, 110, 103, 32, 115, 105, 122, 101, 61, 39, 49, 54, 39, 32, 111, 102, 102, 115, 101, 116, 61, 39, 56, 54, 48, 39, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 68, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 115, 116, 114, 105, 110, 103, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 101, 118, 101, 110, 116, 105, 100, 32, 111, 102, 102, 115, 101, 116, 61, 39,    // | epname>        <string size='16' offset='860'>            <name>Description</name>        </string>        <eventid offset='|
   45, 56, 55, 52, 39, 32, 62, 60, 33, 45, 45, 32, 102, 111, 114, 119, 97, 114, 100, 32, 49, 53, 48, 44, 32, 97, 100, 100, 32, 49, 54, 44, 32, 98, 97, 99, 107, 32, 49, 54, 54, 44, 32, 116, 104, 101, 110, 32, 102, 111, 114, 119, 97, 114, 100, 32, 50, 45, 45, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 83, 101, 116, 32, 69, 118, 101, 110, 116, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 101, 118, 101, 110, 116, 105, 100, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 101, 118, 101, 110,    // | -874' ><!-- forward 150, add 16, back 166, then forward 2-->            <name>Set Event</name>        </eventid>        <even|
   116, 105, 100, 32, 111, 102, 102, 115, 101, 116, 61, 39, 50, 39, 32, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 82, 101, 115, 101, 116, 32, 69, 118, 101, 110, 116, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 101, 118, 101, 110, 116, 105, 100, 62, 10, 32, 32, 32, 32, 60, 47, 103, 114, 111, 117, 112, 62, 10, 60, 47, 115, 101, 103, 109, 101, 110, 116, 62, 10, 10, 60, 115, 101, 103, 109, 101, 110, 116, 32, 111, 114, 105, 103, 105, 110, 61, 39, 48, 39, 32, 115, 112, 97, 99, 101, 61, 39, 50, 53, 51,    // | tid offset='2' >            <name>Reset Event</name>        </eventid>    </group></segment><segment origin='0' space='253|
   39, 62, 10, 32, 32, 32, 32, 60, 105, 110, 116, 32, 115, 105, 122, 101, 61, 39, 52, 39, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 110, 97, 109, 101, 62, 82, 101, 115, 101, 116, 60, 47, 110, 97, 109, 101, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 100, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 62, 67, 111, 110, 116, 114, 111, 108, 115, 32, 114, 101, 108, 111, 97, 100, 105, 110, 103, 32, 97, 110, 100, 32, 99, 108, 101, 97, 114, 105, 110, 103, 32, 110, 111, 100, 101, 32, 109, 101, 109, 111, 114, 121, 46, 32, 66, 111, 97, 114, 100, 32, 109, 117, 115, 116, 32, 98, 101,    // | '>    <int size='4'>        <name>Reset</name>        <description>Controls reloading and clearing node memory. Board must be|
   32, 114, 101, 115, 116, 97, 114, 116, 101, 100, 32, 102, 111, 114, 32, 116, 104, 105, 115, 32, 116, 111, 32, 116, 97, 107, 101, 32, 101, 102, 102, 101, 99, 116, 46, 60, 47, 100, 101, 115, 99, 114, 105, 112, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 109, 97, 112, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 114, 101, 108, 97, 116, 105, 111, 110, 62, 60, 112, 114, 111, 112, 101, 114, 116, 121, 62, 51, 57, 57, 56, 53, 55, 50, 50, 54, 49, 60, 47, 112, 114, 111, 112, 101, 114, 116, 121, 62, 60, 118, 97, 108, 117, 101, 62, 40, 78, 111, 32,    // |  restarted for this to take effect.</description>        <map>            <relation><property>3998572261</property><value>(No |
   114, 101, 115, 101, 116, 41, 60, 47, 118, 97, 108, 117, 101, 62, 60, 47, 114, 101, 108, 97, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 114, 101, 108, 97, 116, 105, 111, 110, 62, 60, 112, 114, 111, 112, 101, 114, 116, 121, 62, 48, 60, 47, 112, 114, 111, 112, 101, 114, 116, 121, 62, 60, 118, 97, 108, 117, 101, 62, 85, 115, 101, 114, 32, 99, 108, 101, 97, 114, 58, 32, 78, 101, 119, 32, 100, 101, 102, 97, 117, 108, 116, 32, 69, 118, 101, 110, 116, 73, 68, 115, 44, 32, 98, 108, 97, 110, 107, 32, 115, 116, 114, 105, 110, 103, 115, 60, 47, 118, 97,    // | reset)</value></relation>            <relation><property>0</property><value>User clear: New default EventIDs, blank strings</va|
   108, 117, 101, 62, 60, 47, 114, 101, 108, 97, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 60, 114, 101, 108, 97, 116, 105, 111, 110, 62, 60, 112, 114, 111, 112, 101, 114, 116, 121, 62, 51, 57, 57, 56, 53, 54, 49, 50, 50, 56, 60, 47, 112, 114, 111, 112, 101, 114, 116, 121, 62, 60, 118, 97, 108, 117, 101, 62, 77, 102, 103, 32, 99, 108, 101, 97, 114, 58, 32, 82, 101, 115, 101, 116, 32, 97, 108, 108, 44, 32, 105, 110, 99, 108, 117, 100, 105, 110, 103, 32, 78, 111, 100, 101, 32, 73, 68, 60, 47, 118, 97, 108, 117, 101, 62, 60, 47, 114, 101, 108,    // | lue></relation>            <relation><property>3998561228</property><value>Mfg clear: Reset all, including Node ID</value></rel|
   97, 116, 105, 111, 110, 62, 10, 32, 32, 32, 32, 32, 32, 32, 32, 60, 47, 109, 97, 112, 62, 10, 32, 32, 32, 32, 60, 47, 105, 110, 116, 62, 10, 60, 47, 115, 101, 103, 109, 101, 110, 116, 62, 10, 10, 60, 47, 99, 100, 105, 62, 10,    // | ation>        </map>    </int></segment></cdi>|
   0
};
const char SNII_const_data[] PROGMEM = "\001TCH Technology\000OpenLCB 16-Input 24-Output Producer/Consumer Node\0001.0.2\0000.7.6";
};

#define SNII_var_data 816
#define SNII_var_offset 20

#include "OlcbArduinoCAN.h"

/* *************************************************
 * EEPROM memory layout
 *     See NodeMemory.h for background info
 *
 * Internal data, not to be reset by user:
 *     0 - 3        Memory state flag
 *     4 - 5        Cycle count
 *     6 - 12       Node ID
 *
 * User configuration data:
 *     13 - 13+8*sizeof(Event)  EventID storage
 *     94 - 113     Node name (zero-term string)
 *     114 - 136     User comment (zero-term string)
 *
 *************************************************** */
#define LAST_EEPROM 12+1588+8*sizeof(Event)

const uint8_t getRead(uint32_t address, int space) {
  if (space == 0xFF) {
    // Configuration definition information
    return pgm_read_byte(configDefInfo+address);
  } else if (space == 0xFE) {
    // All memory
    return *(((uint8_t*)&rxBuffer)+address);
  } else if (space == 0xFD) {
    // Configuration space
    return EEPROM.read(address);
  } else if (space == 0xFC) { // 
    // used by ADCDI for constant data
    return pgm_read_byte(SNII_const_data+address);
  } else if (space == 0xFB) { // 
    // used by ADCDI for variable data
     return EEPROM.read(SNII_var_data+address);
  } else {
    // unknown space
    return 0; 
  }
}
void getWrite(uint32_t address, int space, uint8_t val) {
  if (space == 0xFE) {
    // All memory
    *(((uint8_t*)&rxBuffer)+address) = val;
  } else if (space == 0xFD) {
    // Configuration space
    EEPROM.write(address, val);
  } 
  // all other spaces not written
}

extern "C" {
uint8_t protocolIdentValue[6] = {0xD7,0x58,0,0,0,0};
}

// Events this node can produce or consume, used by PCE and loaded from EEPROM by NM
    Event events[] = {
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event(), 
    Event(), Event(), Event(), Event(),
    Event(), Event(), Event(), Event()
    };
int eventNum = 80;

// output drivers

ButtonLed p22(22, HIGH);
ButtonLed p23(23, HIGH);
ButtonLed p24(24, HIGH);
ButtonLed p25(25, HIGH);
ButtonLed p26(26, HIGH);
ButtonLed p27(27, HIGH);
ButtonLed p28(28, HIGH);
ButtonLed p29(29, HIGH);//24
ButtonLed p30(30, LOW);
ButtonLed p31(31, LOW);
ButtonLed p32(32, LOW);
ButtonLed p33(33, LOW);
ButtonLed p34(34, LOW);
ButtonLed p35(35, LOW);
ButtonLed p36(36, LOW);
ButtonLed p37(37, LOW);//32
ButtonLed p38(38, LOW);
ButtonLed p39(39, LOW);
ButtonLed p40(40, LOW);
ButtonLed p41(41, LOW);
ButtonLed p44(44, LOW);
ButtonLed p45(45, LOW);
ButtonLed p46(46, LOW);
ButtonLed p47(47, LOW);
ButtonLed p2(2, HIGH);//1
ButtonLed p3(3, HIGH);
ButtonLed p4(4, HIGH);
ButtonLed p5(5, HIGH);
ButtonLed p6(6, HIGH);
ButtonLed p7(7, HIGH);
ButtonLed p8(8, HIGH);
ButtonLed p9(9, HIGH);//8
ButtonLed p10(10, HIGH);
ButtonLed p11(11, HIGH);
ButtonLed p12(12, HIGH);
ButtonLed p13(13, HIGH);
ButtonLed p14(14, HIGH);
ButtonLed p17(17, HIGH);
ButtonLed p18(18, HIGH);
ButtonLed p19(19, HIGH);//16

#define ShortBlinkOn   0x00010001L
#define ShortBlinkOff  0xFFFEFFFEL

long patterns[] = {
  ShortBlinkOff,ShortBlinkOn,//1
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,//8
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,//16
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,//24
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,//32
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn,
  ShortBlinkOff,ShortBlinkOn
  
  
};

ButtonLed* buttons[] = {&p2,&p2,&p3,&p3,&p4,&p4,&p5,&p5,&p6,&p6,&p7,&p7,&p8,&p8,&p9,&p9,&p10,&p10,&p11,&p11,&p12,&p12,&p13,&p13,&p14,&p14,&p17,&p17,&p18,&p18,&p19,&p19,&p22,&p22,&p23,&p23,&p24,&p24,&p25,&p25,&p26,&p26,&p27,&p27,&p28,&p28,&p29,&p29,&p30,&p30,&p31,&p31,&p32,&p32,&p33,&p33,&p34,&p34,&p35,&p35,&p36,&p36,&p37,&p37,&p38,&p38,&p39,&p39,&p40,&p40,&p41,&p41,&p44,&p44,&p45,&p45,&p46,&p46,&p47,&p47};

ButtonLed blue(42, LOW);
ButtonLed gold(43, LOW);

void pceCallback(int index){
  // invoked when an event is consumed; drive pins as needed
  // from index
  //
  // sample code uses inverse of low bit of pattern to drive pin all on or all off
  // (pattern is mostly one way, blinking the other, hence inverse)
  //
  buttons[index]->on(patterns[index]&0x1 ? 0x0L : ~0x0L );
}

NodeMemory nm(0);  // allocate from start of EEPROM
void store() { nm.store(&nodeid, events, eventNum); }

PCE pce(events, eventNum, &txBuffer, &nodeid, pceCallback, store, &link);

// Set up Blue/Gold configuration

BG bg(&pce, buttons, patterns, eventNum, &blue, &gold, &txBuffer);

bool states[] = {false, false, false, false,false, false, false, false, false, false, false, false,
false, false, false, false,false, false, false, false, false, false, false, false,
false, false, false, false,false, false, false, false,false, false,false, false,false,false,false,false};

void produceFromInputs() {
  // called from loop(), this looks at pins and 
  // and decides which events to fire.
  // with pce.produce(i);
  // The first event of each pair is sent on button down,
  // and second on button up.
  for (int i = 0; i<eventNum/2; i++) {
    if (states[i] != buttons[i*2]->state) {
      states[i] = buttons[i*2]->state;
      if (states[i]) {
        pce.produce(i*2);
      } else {
        pce.produce(i*2+1);
      }
    }
  }
}

/**
 * Setup does initial configuration
 */
void setup()
{
  // set up serial comm; may not be space for this!
  //delay(250);Serial.begin(BAUD_RATE);logstr("\nTCH Technology \n");
  //Serial.print("free memory in bytes ");
  Serial.println();
  // read OpenLCB from EEPROM
  
  //nm.forceInitAll(); // uncomment if need to go back to initial EEPROM state
  nm.setup(&nodeid, events, eventNum, (uint8_t*) 0, (uint16_t)0, (uint16_t)LAST_EEPROM); 
  
  // set event types, now that IDs have been loaded from configuration
  for (int i=0; i<eventNum/1.6667; i++) {
      pce.newEvent(i,false,true); // produce, consume
  }
  for (int i=eventNum/1.6666; i<eventNum; i++) {
      pce.newEvent(i,true,false); // produce, consume
  }
   Olcb_setup();
}

void loop() {

  bool activity = Olcb_loop();
    if (activity) {
    // blink blue to show that the frame was received
    blue.blink(0x1);
  }
  if (OpenLcb_can_active) {
        gold.blink(0x1);
        OpenLcb_can_active = false;
    }
    if (OpenLcb_can_active) {
        gold.blink(0x1);
        OpenLcb_can_active = false;
    }
  {
    // link not up, but continue to show indications on blue and gold
    blue.process();
    gold.process();
  }

}

