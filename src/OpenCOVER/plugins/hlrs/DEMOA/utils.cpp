/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

const int SUCCESS = 0;
const int ERROR_UTIL = 1;

/******
 * FUNCTION: TextFormat()
 * DESCRIPTION:
 *     Writes a given text to output stream thereby formatting the text
 *     to given linewidth and including given constant text at the
 *     beginning of each line.
 * PARAMETERS:
 *     std::FILE *stream.......output stream
 *     const int width.........maximum width of lines
 *     const char *linebegin...pointer to string to be written at
 *                             beginning of lines
 *     char *text..............pointer to text to be formatted
 * RETURNVALUE: -
 */
void TextFormat(std::FILE *stream, const int width, const char linebegin[],
                char text[])
{
    int cc;
    char *word, *aux;

    /* handle case 'std::strlen(linebegin) >= width' */
    const int offset = (static_cast<int>(std::strlen(linebegin)) >= width)
                           ? (width - std::strlen(linebegin) - 1)
                           : std::strlen(linebegin);

    std::fprintf(stream, "%s", linebegin);
    cc = offset;
    word = std::strtok(text, " ");

    while (word != NULL)
    {
        aux = std::strchr(word, 10); /* ascii('\n') = 10 */
        if (aux != NULL)
        {
            do
            {
                /* i.e. at least one occurance of '\n' in word */
                *aux = '\0';
                ++aux;

                if (cc + static_cast<int>(std::strlen(word)) > width)
                {
                    /* part before '\n' will overflow current line */
                    std::fprintf(stream, "\n%s%s\n%s", linebegin, word,
                                 linebegin);
                    cc = offset;
                }
                else if (cc == offset)
                { /* beginning of line? */
                    std::fprintf(stream, "%s\n%s", word, linebegin);
                }
                else
                {
                    std::fprintf(stream, " %s\n%s", word, linebegin);
                    cc = offset;
                }
                word = aux;
                aux = std::strchr(word, 10); /* another '\n'? */
            } while (aux != NULL);

            if (std::strlen(word) != 0)
            {
                /* part after '\n' is not empty */
                std::fprintf(stream, "%s", word);
                cc += std::strlen(word);
            }
        }
        else
        {
            /* no '\n' in 'word' */
            if (cc + static_cast<int>(std::strlen(word)) + 1 > width)
            {
                /* word + ' ' would overflow current line */
                std::fprintf(stream, "\n%s%s", linebegin, word);
                cc = offset + std::strlen(word);
            }
            else
            {
                /* don't write ' ' in case of writing position is
				 * at beginning of line */
                if (cc == offset)
                {
                    std::fprintf(stream, "%s", word);
                    cc += std::strlen(word);
                }
                else
                {
                    std::fprintf(stream, " %s", word);
                    cc += std::strlen(word) + 1;
                }
            }
        }
        word = std::strtok(NULL, " ");
    }

    std::fprintf(stream, "\n");
}

// TODO: NoEmptyLine() function of sim_data.cpp should be reused here
/******
 * FUNCTION: NoEmptyLine()
 * DESCRIPTION:
 *     Skips following lines of ASCII file if they only
 *     contain white spaces.
 *     Filepointer needs to be at beginning of line.
 * PARAMETER:
 *     std::FILE *stream...pointer to data stream
 * RETURNVALUE:
 *     EOF   if end of file was encountered
 *     1     if next line is a comment
 *     0     otherwise
 */
int NoEmptyLine(std::FILE *stream, const char c)
{
    while (true)
    {
        const long pos = std::ftell(stream);
        char a;
        while ((a = std::fgetc(stream)) != '\n')
        {
            if (a != ' ' && a != '\t')
            {
                if (std::feof(stream))
                {
                    return EOF;
                }

                std::fseek(stream, pos, SEEK_SET);
                if ((a = std::fgetc(stream)) == c)
                {
                    std::ungetc(static_cast<int>(a), stream);
                    return 1;
                }
                // on windows we might be reading the same \n again
                if (a == '\n')
                {
                    if ((a = std::fgetc(stream)) == c)
                    {
                        std::ungetc(static_cast<int>(a), stream);
                        return 1;
                    }
                }
                std::ungetc(static_cast<int>(a), stream);
                return 0;
            }
        }
    }
}

// TODO: GoNextLine() function of sim_data.cpp should be reused here
/******
 * FUNCTION: GoNextLine()
 * DESCRIPTION:
 *     Skip rest of the current line and go to
 *     the beginning of the next one.
 * PARAMETERS:
 *     std::FILE *stream...pointer to data stream
 * RETURNVALUE:
 *     EOF   if end of file was encountered
 *     0     otherwise
 */
int GoNextLine(std::FILE *stream)
{
    // skip rest of current line
    while (!std::feof(stream))
    {
        if (std::fgetc(stream) == '\n')
        {
            return 0;
        }
    }
    return EOF;
}

// TODO: NoComment() function of sim_data.cpp should be reused here
/******
 * FUNCTION: NoComment()
 * DESCRIPTION:
 *     Skips following lines of ASCII file if they are comments,
 *     i.e. if FIRST character is comment character. Filepointer
 *     needs to be at beginning of line.
 * PARAMETERS:
 *     std::FILE *stream...pointer to data stream
 *     const char c...comment character
 * RETURNVALUE:
 *     EOF   if end of file was encountered
 *     0     otherwise
 */
int NoComment(std::FILE *stream, const char c)
{
    char a;

    while ((a = std::fgetc(stream)) == c)
    {
        while ((a = std::fgetc(stream)) != '\n')
            ;
    }
    if (std::feof(stream))
    {
        return EOF;
    }

    // write back read character into stream
    std::ungetc(static_cast<int>(a), stream);

    return 0;
}

// TODO: FindData() function of sim_data.cpp should be reused here
/******
 * FUNCTION: FindData()
 * DESCRIPTION:
 *     Skips all comments and lines containing only white spaces.
 *     File pointer needs to be at beginning of line.
 * PARAMETERS:
 *     std::FILE *stream...pointer to data stream
 *     const char c........comment character
 * RETURNVALUE:
 *     EOF   if end of file has been encountered
 *     0     otherwise
 */
int FindData(std::FILE *stream, const char c)
{
    while (true)
    {
        if (NoComment(stream, c) == EOF)
        {
            return EOF;
        }
        const int iscomment = NoEmptyLine(stream, c);
        if (iscomment == EOF)
        {
            return EOF;
        }
        if (iscomment == 0)
        {
            return 0;
        }
    }
}

// TODO: CountLines() function of sim_data.cpp should be reused here
/******
 * FUNCTION: CountLines()
 * DESCRIPTION:
 *     Counts all lines in an ASCII file skipping comments
 *     and empty lines.
 * PARAMETERS:
 *     std::FILE *stream...pointer to data stream
 *     const char c...comment character
 * RETURNVALUE:
 *     number of lines in file
 */
int CountLines(std::FILE *stream, const char c)
{
    int nlines = 0;

    std::rewind(stream);

    while (!std::feof(stream))
    {
        if (FindData(stream, c) == EOF)
        {
            break;
        }
        if (GoNextLine(stream) == EOF)
        {
            ++nlines;
            break;
        }
        ++nlines;
    }

    std::rewind(stream);

    return nlines;
}

// TODO: CountColumns() function of sim_data.cpp should be reused here
/******
 * FUNCTION: CountColumns()
 * DESCRIPTION:
 *     Find out number of columns in an ASCII file. Only the first
 *     non comment - non empty line is checked.
 * PARAMETERS:
 *     std::FILE *stream...pointer to data stream
 *     const char c........comment character
 * RETURNVALUE:
 *     Number of columns
 */
int CountColumns(std::FILE *stream, const char c)
{
    int number = 0;
    char a = '\0';

    std::rewind(stream);

    if (FindData(stream, c) == EOF)
    {
        std::rewind(stream);
        return 0;
    }

    int flag = false;
    while (a != '\n' && !std::feof(stream))
    {
        a = std::fgetc(stream);
        if ((a == ' ' || a == '\t') && flag)
        {
            flag = false;
        }
        if (a != ' ' && a != '\t' && a != '\n' && !flag)
        {
            ++number;
            flag = true;
        }
    }

    std::rewind(stream);

    return number;
}

GLubyte simple[][13] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36 },
    { 0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66, 0xff, 0x66, 0x66, 0x00, 0x00 },
    { 0x00, 0x00, 0x18, 0x7e, 0xff, 0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e, 0x18 },
    { 0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb, 0xd8, 0x70 },
    { 0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70, 0xd8, 0xcc, 0xcc, 0x6c, 0x38 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c, 0x0e },
    { 0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c },
    { 0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30 },
    { 0x00, 0x00, 0x00, 0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18, 0x00, 0x00 },
    { 0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x03 },
    { 0x00, 0x00, 0x3c, 0x66, 0xc3, 0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66, 0x3c },
    { 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18 },
    { 0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xe7, 0x7e },
    { 0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7, 0x7e },
    { 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c, 0x1c, 0x0c },
    { 0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xff },
    { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e },
    { 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03, 0x03, 0xff },
    { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e },
    { 0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e },
    { 0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06 },
    { 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60 },
    { 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3, 0x7e },
    { 0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18 },
    { 0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe },
    { 0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e },
    { 0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc },
    { 0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff },
    { 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff },
    { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e },
    { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 },
    { 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e },
    { 0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06 },
    { 0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3 },
    { 0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0 },
    { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3 },
    { 0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3 },
    { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e },
    { 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe },
    { 0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c },
    { 0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe },
    { 0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e },
    { 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff },
    { 0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 },
    { 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 },
    { 0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 },
    { 0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3 },
    { 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3 },
    { 0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff },
    { 0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c },
    { 0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60 },
    { 0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18 },
    { 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x70 },
    { 0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0 },
    { 0x00, 0x00, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03, 0x03 },
    { 0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33, 0x1e },
    { 0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0 },
    { 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00 },
    { 0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x00 },
    { 0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc, 0xc6, 0xc0, 0xc0, 0xc0, 0xc0 },
    { 0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78 },
    { 0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfc, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00 },
    { 0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00 },
    { 0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x00 },
    { 0x00, 0x00, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00 },
    { 0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x00, 0x0f, 0x18, 0x18, 0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18, 0x0f },
    { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
    { 0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x1c, 0x18, 0x18, 0x18, 0xf0 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00, 0x00 }
};

GLuint fontOffset;

void glMakeSimpleFont()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    fontOffset = glGenLists(128);
    for (GLuint i = 32; i < 127; ++i)
    {
        glNewList(i + fontOffset, GL_COMPILE);
        glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, simple[i - 32]);
        glEndList();
    }
}

void glPrintString(const char s[])
{
    glPushAttrib(GL_LIST_BIT);
    glListBase(fontOffset);
    glCallLists(std::strlen(s), GL_UNSIGNED_BYTE,
                reinterpret_cast<const GLubyte *>(s));
    glPopAttrib();
}
