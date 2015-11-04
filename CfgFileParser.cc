/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 2009  Joakim Ek <joakim@pjek.se>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "CfgFileParser.h"

#include <iostream>
#include <assert.h>
#include <cstring>
#include <stdlib.h>

namespace gb = gmb::memory;
using namespace std;

// ## class SearchData ##

SearchData::SearchData()
  : m_pf(NULL)
{ }

SearchData::~SearchData()
{ }

CfgFileParser::CfgFileParser()
  : m_line(0)
{ }

CfgFileParser::~CfgFileParser()
{ }

char_ptr CfgFileParser::read_line()
{
   // reads the next line from the file and increases the line counter
   // prints error message if no characters were read.
   // returns: string or NULL if error.

   char_ptr str = make_char_ptr(MAX_CFG_LINE_LENGTH);
   assert(str);
   
   // read next line
   m_infile.getline(str.get(), MAX_CFG_LINE_LENGTH);

   // increase line counter
   m_line++;
   
   // read less than zero chars?
   if (m_infile.gcount() <= 0)
   {
      // found EOF
      return char_ptr();
   }

   return str;
}

int CfgFileParser::read_item()
{
  // reads the color for the reg exps, then eads the reg exps and makes
  // SearchData instances for them and adds to the m_items_list
  // only reads and processes ONE item
  // returns number of SearchData instances added to the list

  int nr_items_added = 0;

  // is there a list?
  //assert (m_items_list != NULL);

  // is there a open file?
  assert (m_infile);

  // read color
  char_ptr color = read_color();

  if (!color)
  {
    // didn't get a color

    // quit, return 0
    return 0;
  }

  // TODO: add action reading
  // read action

  if (read_left() == 0)
  {
    // found '{'

    // loop until '}'
    while (1)
    {
      // read string
      char_ptr regexp = read_regexp();
      if (regexp)
      {
        // check if it's a '}'
        if (regexp.get()[0] == '}')
        {
          // stop looping
          break;
        }

        // has a regexp.. make a SearchData item

        gb::shared_ptr<SearchData> searchdata(new SearchData());
        // check allocation
        assert (searchdata);

        // set color
        searchdata->set_color(color);

        // allocate memory to the pattern storage buffer
        searchdata->m_preg = make_regex_t_ptr();
        // check allocation
        assert(searchdata->m_preg);

        // make compiled pattern buffer
        if (regcomp(searchdata->m_preg.get(), regexp.get(), REG_EXTENDED) != 0)
        {
          // failed to make compiled reg exp pattern
          cout << "colortail: Failed to make compiled reg exp pattern for "
          << "reg exp in config file " << m_filename.get() << " at line "
          << m_line << ". Skipping this line." << endl;
        }
        else
        {
          // TODO: set callback fkn
          // TODO: set param to callback fkn

          // add the search data item to the items list
          m_items_list.add(searchdata);
          // increase items added counter
          nr_items_added++;
        }
      }
      else
      {
        // error reading string, eof maybe..
        // stop looping
        break;
      }
    }
  }

  return nr_items_added;
}

char_ptr CfgFileParser::read_color()
{
  // reads and skips lines until a 'COLOR' statement is found,
  // it then extracts the color after 'COLOR' and returns a new string
  // containing the ANSI color code for that string.
  // RETURNS: the new string, NULL on error.

  // is there a open file?
  assert (m_infile);

  while (1)
  {
    // read line
    char_ptr tmp = read_line();
    if (!tmp)
    {
      // found EOF
      return tmp;
    }

    // got a line to look at

    // process line if it's not empty or doesn't starts with a '#'
    if (tmp.get()[0] != '\0' && tmp.get()[0] != '#')
    {
      if (strncmp(tmp.get(), "COLOR", 5) != 0)
      {
        cout << "colortail: Error in config file: " << m_filename.get()
        << " at line " << m_line << ". Skipping this line." << endl;
      }
      else 
      {
        int linepos = 5;
        char_ptr color = make_char_ptr(20);

        // skip all spaces
        while (tmp.get()[linepos] == ' ')
        {
          linepos++;
        }

        // read which color it is
        int found_color = 0;

        if (strncmp(&tmp.get()[linepos], "black", 5) == 0) {
          strcpy(color.get(), BLACK);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "red", 3) == 0) {
          strcpy(color.get(), RED);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "green", 5) == 0) {
          strcpy(color.get(), GREEN);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "yellow", 6) == 0) {
          strcpy(color.get(), YELLOW);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "blue", 4) == 0) {
          strcpy(color.get(), BLUE);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "magenta", 7) == 0) {
          strcpy(color.get(), MAGENTA);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "cyan", 4) == 0) {
          strcpy(color.get(), CYAN);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "white", 6) == 0) {
          strcpy(color.get(), WHITE);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightblack", 6+5) == 0) {
          strcpy(color.get(), BRIGHTBLACK);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightred", 6+3) == 0) {
          strcpy(color.get(), BRIGHTRED);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightgreen", 6+5) == 0) {
          strcpy(color.get(), BRIGHTGREEN);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightyellow", 6+6) == 0) {
          strcpy(color.get(), BRIGHTYELLOW);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightblue", 6+4) == 0) {
          strcpy(color.get(), BRIGHTBLUE);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightmagenta", 6+7) == 0) {
          strcpy(color.get(), BRIGHTMAGENTA);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightcyan", 6+4) == 0) {
          strcpy(color.get(), BRIGHTCYAN);
          found_color = 1;
        }

        if (strncmp(&tmp.get()[linepos], "brightwhite", 6+5) == 0) {
          strcpy(color.get(), BRIGHTWHITE);
          found_color = 1;
        }

        if (found_color == 0)
        {
          // didn't found color
          cout << "colortail: Don't recognize color in config file: "
          << m_filename.get() << " at line " << m_line << endl;

          // error, return NULL
          return char_ptr();
        }

        return color;
      }
      // free memory for read line
    }
  }
  // should never execute this
  return char_ptr();
}

int CfgFileParser::read_left()
{
  // reads lines (skips empty and comments lines) until a '{' is found
  // RETURNS: 0 if a '{' is found, 1 if not found, 2 if error

  // is there a open file?
  assert (m_infile);

  while (1)
  {
    // read line
    char_ptr tmp = read_line();
    if (!tmp)
    {
      // error reading line
      cout << "colortail: Error reading line in config file: "
      << m_filename.get() << " at line " << m_line << "." << endl;
      // error, return 2
      return 2;
    }

    // got a line to look at

    // process line if it's not empty or doesn't starts with a '#'
    if (tmp.get()[0] != '\n' && tmp.get()[0] != '#')
    {
      if (tmp.get()[0] == '{')
      {
        return 0;
      }
      else
      {
        // not a '{'
        cout << "colortail: Error, expected '{' but found '"
        << tmp.get()[0] << "' in config file: " << m_filename.get()
        << " at line " << m_line << "." << endl;
        // free mem

        return 1;
      }
    }

  }

  // should never get this far
  return 1;
}

char_ptr CfgFileParser::read_regexp()
{
  // reads and skips empty and comments lines until a regexp is found
  // RETURNS: a new string containing the reg exp, NULL on error

  // is there a open file?
  assert (m_infile);

  while (1)
  {
    // read line
    char_ptr tmp = read_line();

    if (!tmp)
    {
      // error reading line
      cout << "colortail: Error reading line in config file: "
      << m_filename.get() << " at line " << m_line << "." << endl;

      // error, return NULL
      return tmp;
    }

    // got a line

    // process line if it's not empty or doesn't starts with a '#'
    if (tmp.get()[0] != '\n' && tmp.get()[0] != '#')
    {
      // found a string, return it
      return tmp;
    }

    // this is a empty line or a comment, skip it

  }

  // should never get this far
  return char_ptr();
}
	 

int CfgFileParser::parse(const char *filename)
{
   // parses the cfg file and sets up the list of SearchData elements
   // returns number of SearchData items created

   // is there a list?
   if (!m_items_list.is_empty())
   {
      // delete it
      m_items_list.delete_all_values();
   }

   assert(m_items_list.is_empty());

   // try to open the file
   m_infile.open(filename, ios::in);

   if (!m_infile)
   {
      // open failed
      cout << "colortail: Failed to open config file: " << filename << endl;
      return 0;
   } 
   // save filename of config file
   m_filename = make_char_ptr(strlen(filename) + 1);
   strcpy(m_filename.get(), filename);
   

   int items_counter = 0;
   
   // read the items
   while (!m_infile.eof())
   {
      // read in a item
      int n = read_item();

      items_counter += n;
   }
   
   return items_counter;
}
      
void CfgFileParser::get_items_list(List<gb::shared_ptr<SearchData> > &list)
{
  ListIterator<gb::shared_ptr<SearchData> > iter(m_items_list);

  for(iter.init();
      !iter;
      ++iter) {

    list.add(iter());
  }

  m_items_list.delete_all_values();
}

