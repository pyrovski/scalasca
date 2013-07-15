#ifndef CUBEPARSER_DRIVER_H
#define CUBEPARSER_DRIVER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "Cube.h"

// forward declaration
class ParseContext;


/** The cubeparser namespace is used to encapsulate the parser classes
 * cubeparser::Cube3Parser, cubeparser::Cube2Parser, 
 * cubeparser::Cube3Scanner, cubeparser::Cube2Scanner
 *  and cubeparser::Driver */

namespace cubeparser {

  class Cube2Scanner;
  class Cube3Scanner;

/** The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets its token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */

class Driver
{
private:
    char * currentlocale;

public:
    /// construct a new parser driver context
    Driver();
    ~Driver();

    /// stream name (file or input stream) used for error messages.
    std::ostringstream errorStream;
    std::string streamname;

    /** Invoke the scanner and parser for a stream.
     * @param in	input stream
     * @param sname	stream name for error messages
     * @return		true if successfully parsed
     */
    void parse_stream(std::istream& in, cube::Cube& cubeObj);

    void parse_stream_dynamic(std::istream& in, cube::Cube& cubeObj, std::string fileName);
    void parse_matrix(std::istream& in, cube::Cube& cubeObj, int index);
    void parse_matrix_pos(std::istream& in);

    // To demonstrate pure handling of parse errors, instead of
    // simply dumping them on the standard error output, we will pass
    // them to the driver using the following two member functions.

    /** Error handling with associated line number. This can be modified to
     * output the error e.g. to a dialog box. */
    void error(const class location& l, const std::string& m);

    /** 
    This call is used only to  place an error message in to errorstream without
    throwing an exception. Used to build full error message in Cube3Parse.yy
    */
    void error_just_message(const std::string& m);

    /** General error handling. This can be modified to output the error
     * e.g. to a dialog box. */
    void error(const std::string& m);

    /** Info output*/
    void info(const std::string& m);

    /** Pointer to the current lexer instance, this is used to connect the
     * parser to the scanner. It is used in the yylex macro. */
    class Cube3Scanner* cube3Lexer;
    class Cube2Scanner* cube2Lexer;
    class Cube2Parser* cube2Parser;
    class Cube3Parser* cube3Parser;

    /** Reference to the context filled during parsing of the
     * expressions. */
    class ParseContext* parseContext;

    std::vector<long int> metricPos;
    std::vector<double> metricSum;


};

} // namespace cubeparser

#endif // CUBEPARSER_DRIVER_H
