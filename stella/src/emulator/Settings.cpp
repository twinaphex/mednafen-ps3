//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2011 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: Settings.cxx 2245 2011-06-02 20:53:01Z stephena $
//============================================================================

//ROBO: For mednafen settings
#include "src/mednafen.h"
#include "src/settings-driver.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "bspf.hxx"

//ROBO: No OSystem
//#include "OSystem.hxx"
#include "Version.hxx"

#include "Settings.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//ROBO: No OSystem
//Settings::Settings(OSystem* osystem)
Settings::Settings(void* osystem)
//  : myOSystem(osystem)
{
  // Add this settings object to the OSystem
//ROBO: No OSystem
//  myOSystem->attach(this);

  // Add options that are common to all versions of Stella
  setInternal("video", "soft");

  // OpenGL specific options
  setInternal("gl_filter", "nearest");
  setInternal("gl_aspectn", "100");
  setInternal("gl_aspectp", "100");
  setInternal("gl_fsmax", "false");
  setInternal("gl_lib", "libGL.so");
  setInternal("gl_vsync", "false");
  setInternal("gl_texrect", "false");
//  setInternal("gl_accel", "true");

  // Framebuffer-related options
  setInternal("tia_filter", "zoom2x");
  setInternal("fullscreen", "0");
  setInternal("fullres", "auto");
  setInternal("center", "false");
  setInternal("grabmouse", "true");
  setInternal("palette", "standard");
  setInternal("colorloss", "false");
  setInternal("timing", "sleep");
  setInternal("uimessages", "true");

  // TV filter options
  setInternal("tv_tex", "off");
  setInternal("tv_bleed", "off");
  setInternal("tv_noise", "off");
//  setInternal("tv_curve", "false");  // not yet implemented
  setInternal("tv_phos", "false");

  // Sound options
  setInternal("sound", "true");
  setInternal("fragsize", "512");
  setInternal("freq", "31400");
  setInternal("tiafreq", "31400");
  setInternal("volume", "100");
  setInternal("clipvol", "true");

  // Input event options
  setInternal("keymap", "");
  setInternal("joymap", "");
  setInternal("joyaxismap", "");
  setInternal("joyhatmap", "");
  setInternal("combomap", "");
  setInternal("joydeadzone", "13");
  setInternal("joyallow4", "false");
  setInternal("usemouse", "true");
  setInternal("dsense", "5");
  setInternal("msense", "7");
  setInternal("sa1", "left");
  setInternal("sa2", "right");
  setInternal("ctrlcombo", "true");

  // Snapshot options
  setInternal("ssdir", "");
  setInternal("sssingle", "false");
  setInternal("ss1x", "false");
  setInternal("ssinterval", "2");

  // Config files and paths
  setInternal("romdir", "");
  setInternal("statedir", "");
  setInternal("cheatfile", "");
  setInternal("palettefile", "");
  setInternal("propsfile", "");
  setInternal("eepromdir", "");
  setInternal("cfgdir", "");

  // ROM browser options
  setInternal("uselauncher", "true");
  setInternal("launcherres", "640x480");
  setInternal("launcherfont", "medium");
  setInternal("launcherexts", "allfiles");
  setInternal("romviewer", "0");
  setInternal("lastrom", "");

  // UI-related options
  setInternal("debuggerres", "1030x690");
  setInternal("uipalette", "0");
  setInternal("listdelay", "300");
  setInternal("mwheel", "4");

  // Misc options
  setInternal("autoslot", "false");
  setInternal("showinfo", "1");
  setInternal("tiadriven", "false");
  setInternal("ramrandom", "true");
  setInternal("avoxport", "");
  setInternal("stats", "false");
  setInternal("audiofirst", "true");
  setInternal("fastscbios", "false");
  setExternal("romloadcount", "0");
  setExternal("maxres", "");

  // Debugger options
  setInternal("resolvedata", "auto");
  setInternal("gfxformat", "2");
  setInternal("showaddr", "true");
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Settings::~Settings()
{
  myInternalSettings.clear();
  myExternalSettings.clear();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::loadConfig()
{
  //ROBO: Kill the body of this for brevity
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Settings::loadCommandLine(int argc, char** argv)
{
  //ROBO: Kill the body of this for brevity
  return "";
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::validate()
{
  //ROBO: Kill the body of this for brevity
  return;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::usage()
{
  //ROBO: Kill the body of this for brevity
  return;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::saveConfig()
{
  //ROBO: Kill the body of this for brevity
  return;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::setInt(const string& key, const int value)
{
  ostringstream stream;
  stream << value;

  if(int idx = getInternalPos(key) != -1)
    setInternal(key, stream.str(), idx);
  else
    setExternal(key, stream.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::setFloat(const string& key, const float value)
{
  ostringstream stream;
  stream << value;

  if(int idx = getInternalPos(key) != -1)
    setInternal(key, stream.str(), idx);
  else
    setExternal(key, stream.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::setBool(const string& key, const bool value)
{
  ostringstream stream;
  stream << value;

  if(int idx = getInternalPos(key) != -1)
    setInternal(key, stream.str(), idx);
  else
    setExternal(key, stream.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::setString(const string& key, const string& value)
{
  setInternal(key, value);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::getSize(const string& key, int& x, int& y) const
{
  char c;
  string size = getString(key);
  istringstream buf(size);
  buf >> x >> c >> y;
  if(c != 'x')
    x = y = -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Settings::getInt(const string& key) const
{
  //ROBO: Use mednafen for the settings
//  MDFN_GetSettingI(key.c_str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
float Settings::getFloat(const string& key) const
{
  //ROBO: Use mednafen for the settings
//  MDFN_GetSettingF(key.c_str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Settings::getBool(const string& key) const
{
  //ROBO: Use mednafen for the settings
//  MDFN_GetSettingB(key.c_str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const string& Settings::getString(const string& key) const
{
  // Try to find the named setting and answer its value
  //ROBO: Use mednafen for the settings
//  MDFN_GetSettingS(key.c_str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Settings::setSize(const string& key, const int value1, const int value2)
{
  ostringstream buf;
  buf << value1 << "x" << value2;
  setString(key, buf.str());
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Settings::getInternalPos(const string& key) const
{
  //ROBO: Kill the body of this for brevity
  return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Settings::getExternalPos(const string& key) const
{
  //ROBO: Kill the body of this for brevity
  return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Settings::setInternal(const string& key, const string& value,
                          int pos, bool useAsInitial)
{
	//ROBO: Use mednafen for the settings
//	MDFNI_SetSetting(key.c_str(), value.c_str());

	//ROBO: Kill the body of this for brevity
	return -1;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int Settings::setExternal(const string& key, const string& value,
                          int pos, bool useAsInitial)
{
	//ROBO: Kill the body of this for brevity
//	assert(false);
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Settings::Settings(const Settings&)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Settings& Settings::operator = (const Settings&)
{
  assert(false);

  return *this;
}
