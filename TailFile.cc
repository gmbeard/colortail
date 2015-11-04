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

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "TailFile.h"

#define STRCMP_EQ 0
namespace gb = gmb::memory;
using namespace std;

TailFile::TailFile()
  : m_file(NULL),
    m_position(0),
    reopening(0)
{
   memset(&m_file_stats, 0, sizeof(struct stat));
}

TailFile::~TailFile()
{
   if (m_file)
   {
      fclose(m_file);
   }
}

int TailFile::reopen()
{
  if (m_file)
    fclose(m_file);
  m_file = NULL;

  m_position = 0;
  memset(&m_file_stats, 0, sizeof(struct stat));
  reopening = 1;

  char_ptr tmp = make_char_ptr(strlen(m_filename.get()) + 1);
  
  strcpy(tmp.get(), m_filename.get());

  gb::shared_ptr<Colorizer> tmp_color;

  if(m_colorizer) {
    tmp_color = gb::shared_ptr<Colorizer>(new Colorizer(*m_colorizer));
  }

  int ret = open(tmp.get(), m_colorizer);

  reopening = 0;
  return ret;
}

int TailFile::open(char *filename, gb::shared_ptr<Colorizer> colorizer)
{
   // Opens the file to tail. And sets the colorizer
   // Prints error message if failed to open file.
   // returns: 0 - success
   //          1 - failure

   if (filename == NULL)
   {
      cout << "colortail: Filename was NULL" << endl;
      return 1;
   }
   else
   {
      // save filename
      m_filename = make_char_ptr(strlen(filename) + 1);
      strcpy(m_filename.get(), filename);

      // tries to open the file
      if (strcmp (filename, "-") == 0)
      {
         m_file = stdin;
      }
      else
         m_file = fopen(filename, "r");

      if (m_file == NULL)
      {
	 // open failed
	 cout << "colortail: Failed to open file: " << filename << endl;
	 return 1;
      }

      m_colorizer = colorizer;
      // set the saved stream position used to see if the file has
      // changed size to the end of the file
      m_position = reopening ? 0 : end_of_file_position();
   }
   return 0;
}

void TailFile::print(int n)
{
  // print and colorize last n rows of the file

  // is a file open
  if (m_file == NULL)
  {
    // no file open
    return;
  }

  find_position(n);

  //   const bufSize = 1024;
  char buf[MAX_CHARS_READ];

  while(NULL != fgets(&buf[0], sizeof(buf), m_file)) {
    print_to_stdout(&buf[0]);
  }

}

void TailFile::printFilename()
{
   // prints the filename to stdout

   cout << "==> " << m_filename.get() << " <==" << endl;
}

char* TailFile::get_filename()
{
   // returns the filename
   return m_filename.get();
}

void TailFile::printAll()
{
   // print the complete file, for debugging
   const int bufSize = 1024;
   char buf[bufSize];
}


void TailFile::find_position(int n)
{
   // sets the file to the position after the n:th return
   // from the end of the file
   
   // check if file is not open
   if (m_file == NULL)
   {
      // no file open
      return;
   }

   // check if zero rows
   if (n == 0)
   {
      // zero rows
      // set position to end of file
      fseek(m_file, 0, SEEK_END);
      return;
   }
   
   const int bufSize (2048);
   // buffer for chars
   char buf[bufSize];

   // bytes to read
   int bytesToRead = bufSize;

   // position in file from beginning
   long pos = 0;

   // go to end of file
   fseek(m_file, 0, SEEK_END);

   // set pos to last position
   pos = ftell(m_file);

   while (pos != 0)
   {   
      // sub bufSize-1 from position
      pos -= (bufSize-1);

      // check if pos < 0
      if (pos < 0)
      {
	 // pos below zero
	 
	 // calc new bytesToRead (pos is < 0)
	 bytesToRead += pos;
	 
	 // pos below zero, set pos to zero
	 pos = 0;
      }
   
      // goto that position
      fseek(m_file, pos, SEEK_SET);

      // read bytesToRead bytes from the file
      // note: use fgetc, because fgets stops at newlines
      for (int j = 0 ; j < bytesToRead-1 ; j++)
      {
	 buf[j] = fgetc(m_file);
      }
      // null terminate the string
      buf[bytesToRead-1] = '\0';

      // search through the character buffer
      for (int i = bytesToRead-1 ; i >= 0 ; i--)
      {
	 // is it a CR
	 if (buf[i] == '\n')
	 {
	    // found return
	    // decrease n
	    n--;
	    // enough returns
	    if (n < 0)
	    {
	       // goto position after the return
	       fseek(m_file, pos + i + 1, SEEK_SET);
	       return;
	    }
	 }
      }
   }

   // if execution gets here, then all the returns wasn't found
   // set position to beginning of file
   fseek(m_file, 0, SEEK_SET);
}

long TailFile::end_of_file_position()
{
   // returns the position for the end of the current file ie
   // the size of the file.
   // doesn't modify the current position
   
   // check if file isn't open
  if (m_file == NULL && reopen())
   {
      // no file open, return 0
      return 0;
   }

   struct stat file_stats;

   // get the stats for the current file
   int ret = stat(m_filename.get(), &file_stats);

   if (ret != 0)
   {
     // some error, return 0
     return 0;
   }

   if (m_file_stats.st_ino && file_stats.st_ino != m_file_stats.st_ino)
     {
       if (!reopen())
	 {
	   int ret = stat(m_filename.get(), &file_stats);
	   if (ret)
	     return 0;
	 }
     }

   memcpy(&m_file_stats, &file_stats, sizeof(struct stat));

   // return the size
   return file_stats.st_size;
}

int TailFile::more_to_read()
{
   // returns the number of bytes that can be read from the file
   // or 0 if nothing has been added to the file
   // updates the saved position if the file has been truncated.
   // RETURNS: number of bytes or 0

   // check if no file open
   if (m_file == NULL)
   {
      // no file open, return 0
      return 0;
   }

   // get the end of file position
   long end = end_of_file_position();

   // check if file is truncated
   if (end < m_position)
   {
      // yes, truncated
      // set the saved position to the new (lesser) stream position
      m_position = end;
      // nothing added to the file, just removed
      return 0;
   }

   return end - m_position;
}

void TailFile::follow_print(int n, int verbose, char *last_filename)
{
  // Reads n characters from the file and search from the beginning after a
  // '\n'. If it is found that line is colorized and printed, and the
  // current stream position is uppdated. If a line is found and the
  // verbose flag is set. The filename is printed in ==> <== :s if it
  // isn't the same file as the last line was printed from.
  // If it isn't found, nothing is printed and the stream position
  // isn't changed.

  // check if a file isn't open
  if (m_file == NULL)
  {
    // no file open
    // just return
    return;
  }

  //  Clear any error flags on the file pointer...
  clearerr(m_file);

  static char buf[MAX_CHARS_READ];

  // Loop for each line (or block of MAX_CHARS_READ size) ...
  while(NULL != fgets(&buf[0], MAX_CHARS_READ, m_file)) {
    
    //  If the verbose flag is set, and we've not
    //  already logged the filename, or logged a
    //  different file since, then print the header...
    if (verbose &&
        (last_filename == NULL || 
          STRCMP_EQ != strcmp(last_filename, m_filename.get()) )) {

      printFilename();
    }

    //  Send the block to the colorizer...
    print_to_stdout(&buf[0]);

    //  Save the file position at this point (?)...
    m_position = ftell(m_file);

  }   
}

void TailFile::print_to_stdout(const char *str)
{
   // checks if there is a colorizer. If so the string is colorized
   // before it's printed. If not the string isn't colorized.

   // check that str is something
   if (str == NULL)
   {
      return;
   }

   // check if we have a colorizer
   if (m_colorizer)
   {
      // colorize the string
      string res = m_colorizer->colorize(str);
      // print the new colorized string
      cout.write(res.c_str(), res.size());
   }
   else
   {
      // don't colorize

      // print the line
      cout.write(str, strlen(str));
   }
}

