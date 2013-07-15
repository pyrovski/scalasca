#include <cassert>
#include <cstdlib>
#include <cstring>
#include <locale.h>

#include "Driver.h"
#include "Cube2Parser.h"
#include "Cube3Parser.h"
#include "Cube2Scanner.h"
#include "Cube3Scanner.h"
#include "cube_error.h"
#include "Metric.h"
#include "Cnode.h"
#include "Thread.h"

/**
* \file Driver.cpp
* \brief Implementation of all components of Driver to scan and parse a .cube file and to create CUBE.
*/

/**
* Defines all components of Driver to scan and parse a .cube file and to create CUBE.
*/
namespace cubeparser {
  

Driver::Driver() : errorStream(std::ostringstream::out){
  parseContext = new ParseContext();
  cube2Lexer = NULL;
  cube3Lexer = NULL;
  cube2Parser = NULL;
  cube3Parser = NULL;
  metricPos.clear();
  metricSum.clear();

  currentlocale = setlocale(LC_ALL, NULL);
  setlocale(LC_ALL, "C");
}

Driver::~Driver(){
  setlocale(LC_ALL, currentlocale);
  delete parseContext;
}

enum ParseState {INIT,SEVERITY,SEVERITY_MATRIX,SEVERITY_END,MATRIX,MATRIX_ROW,MATRIX_END,ROW,ROW_END};

#define SIZE 1024

void Driver::parse_matrix_pos(std::istream& in){

  metricPos.clear();
  metricSum.clear();

  in.clear();

  in.seekg (0, std::ios::end);
  long int length = in.tellg();
  in.seekg (0, std::ios::beg);  
  in.seekg(0);

  int size = SIZE;

  char c[SIZE];
  char c2[100];  
  std::streampos charCounter = 0;
  long int threshold = length/100;
  long int diff = threshold;
  int pos = size;

  ParseState state = INIT;

  int metricId = -1;
  double value;
  char* remain;

  std::string str;

  while (true){

    if (pos>=size){
      if (in.eof()) {
	info("Unexpected EOF\n");
	goto Error;
      }
      if (parseContext->dynamicMetricLoading){
	if (charCounter>=threshold){
	  if (metricId!=-1){
	    errorStream << "\rfound metric " <<  metricId; 
	    errorStream << " (" << (long int)(100.0 * (double)in.tellg()/(double)length) << "%)";
	  } else {
	    errorStream << "\rSearching for metrics...";
	  }
	  info("");
	  threshold += diff;
	}
      }

      in.read(c,size);
      if (in.eof()){
	in.clear();
	in.seekg(charCounter);
	int i = 0;
	while (!in.eof()){
	  in.get(c[i++]);
	  charCounter += 1;
	}
	charCounter -= 1;
	size = i;
	in.clear();
      } else charCounter += size;

      assert(charCounter==in.tellg());
      pos = 0;

    }

    //search for <severity>
    
    if (state==INIT){
      
      while (pos<size){
	if (c[pos++]=='<') break;
      }
      if (pos>=size && c[pos-1]!='<') continue;
      
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='s') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='e') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='v') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='e') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='r') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='i') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='t') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='y') continue;
      ++pos;
      if (pos<size) c2[0] = c[pos];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }
      if (c2[0]!='>') continue;
      ++pos;

      state = SEVERITY;
      if (pos>=size) continue;
    } 
    
    //after "<severity>" was found,
    //search for "<matrix..." or for "</severity>"
    
    if (state==SEVERITY){

      while (pos<size){
	if (c[pos]==' ' || c[pos]=='\n' || c[pos]=='\t'){
	  ++pos;
	} else if (c[pos]=='<') {
	  ++pos;
	  break;
	} else {
	  errorStream << "Expected <matrix metricId=\"int\"> or </severity>\n"; 
	  goto Error;
	}
      }
      if (pos>=size && c[pos-1]!='<') continue;	

      if (pos<size) c2[0] =  c[pos++];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }

      if (c2[0]=='m') state = SEVERITY_MATRIX;
      else if (c2[0]=='/') state = SEVERITY_END;
      else {
	errorStream << "Expected <matrix metricId=\"int\"> or </severity>\n"; 
	goto Error;
      }
    } 

    //a matrix starts here

    if (state==SEVERITY_MATRIX){

      for (int i=0; i<16; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
      c2[16] = '\0';
      pos += 16;

      if (strcmp(c2,"atrix metricId=\"")!=0) {
	errorStream << "Expected <matrix metricId=\"int\">\n"; 
	goto Error;
      }

      int i = 0;
      while (true) {
	if (pos<size) c2[i] = c[pos++];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
	if (c2[i]=='\"'){
	  if (pos<size) c2[i] = c[pos++]; else {
	    in.get(c2[i]);
	    charCounter += 1;
	  }
	  if (c2[i]!='>') {
	    errorStream << "Expected \"> after the metric id\n";
	    goto Error;
	  }
	  c2[i] = '\0';
	  break;
	}
	++i;
      }

      metricId = (int)strtol(c2,&remain,10); 
      if (remain[0]!='\0') {
	errorStream << "Metric id is not an integer\n";
	goto Error;
      }
      if (parseContext->dynamicMetricLoading){
	errorStream << "\nfound metric " <<  metricId; 
	info("");
      }

      while ((int) (metricPos.size()) <=metricId){
	metricPos.push_back(-1);
	metricSum.push_back(0.0);
      }

      long int index = 1 + charCounter;
      if (pos<size) index -= (size-pos);
      metricPos[metricId] = index;

      state = MATRIX;
      if (pos>=size) continue;

    }

    //the severity ends here

    if (state==SEVERITY_END){

      for (int i=0; i<9; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
      c2[9] = '\0';
      pos += 9;

      if (strcmp(c2,"severity>")!=0) {
	errorStream << "Expected </severity>\n"; 
	goto Error;
      }
      
      break;
    }


    //we are in a matrix now

    if (state==MATRIX){

      while (pos<size){
	if (c[pos]==' ' || c[pos]=='\n' || c[pos]=='\t'){
	  ++pos;
	} else if (c[pos]=='<') {
	  ++pos;
	  break;
	} else {
	  errorStream << "Expected </matrix> or <row cnodeid=\"int\">\n";
	  goto Error;
	}
      }
      if (pos>=size && c[pos-1]!='<') continue;	

      if (pos<size) c2[0] =  c[pos++];
      else {
	in.get(c2[0]);
	charCounter += 1;
      }

      if (c2[0]=='r') state = MATRIX_ROW;
      else if (c2[0]=='/') state = MATRIX_END;
      else {
	errorStream << "Expected </matrix> or <row cnodeid=\"int\">\n";
	goto Error;
      }

    } 
    //a row starts here

    if (state==MATRIX_ROW){

      for (int i=0; i<12; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
      c2[12] = '\0';
      pos += 12;

      if (strcmp(c2,"ow cnodeId=\"")!=0) {
	errorStream << "Expected <row cnodeid=\"\n";
	goto Error;
      }

      int i = 0;
      while (true) {
	if (pos<size) c2[i] = c[pos++];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
	if (c2[i]=='\"'){
	  if (pos<size) c2[i] = c[pos++]; else {
	    in.get(c2[i]);
	    charCounter += 1;
	  }
	  if (c2[i]!='>') {  
	    errorStream << "Expected int\">\n";	    
	    goto Error;
	  }
	  c2[i] = '\0';
	  break;
	}
	++i;
      }

      state = ROW;
      if (pos>=size) continue;

    }

    //the matrix ends here

    if (state==MATRIX_END){

      for (int i=0; i<7; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
      c2[7] = '\0';
      pos += 7;

      if (strcmp(c2,"matrix>")!=0) {
	errorStream << "Expected </matrix>\n";
	goto Error;
      }
      
      state = SEVERITY;
      if (pos>=size) continue;
    }

    //we are in a row now

    if (state==ROW){

      while (pos<size){

	while (pos<size){
	  if (c[pos]==' ' || c[pos]=='\n' || c[pos]=='\t'){
	    ++pos;
	  } else break;
	}
	if (pos>=size) break;	

	if (c[pos]=='<'){ state = ROW_END; pos++; break;}

	int i = 0;
	while (true) {
	  if (pos<size) c2[i] = c[pos++];
	  else {
	    in.get(c2[i]);
	    charCounter += 1;
	  }
	  if (c2[i]=='\n'){
	    c2[i] = '\0';
	    break;
	  }
	  ++i;
	}
	// LOCALE Dependend
	// All values are expected in locale C
	value = strtod(c2,&remain); 
	if (remain[0]!='\0') {
	  errorStream << "Expected double or </row>\n";
	  goto Error;
	}

	if (value!=0.0){
	  metricSum[metricId] += value;
	}

      }
      if (pos>=size && state==ROW) continue;

    }

    //a row ends here

    if (state==ROW_END){

      for (int i=0; i<5; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else {
	  in.get(c2[i]);
	  charCounter += 1;
	}
      c2[5] = '\0';
      pos += 5;

      if (strcmp(c2,"/row>")!=0) {
	errorStream << "Expected </row>\n";
	goto Error;
      }
      state = MATRIX;
      //if (pos>=size) continue;
    }

  }

  info("");
  errorStream.str("");
  return;

 Error:
  long int n = (long int)(in.tellg()) - (size-pos-1);
  errorStream << "Syntax error at file position " << n << "\n";
  errorStream << "before: ";
  for (long int i=-20; i<0; i++)
    if (pos+i>=0) errorStream << c[pos+i];
  errorStream << "\nerror: ";
  for (long int i=0; i<=20; i++)
    if (pos+i<size) errorStream << c[pos+i];
  errorStream << "\n";
  error(errorStream.str());

}


void Driver::parse_matrix(std::istream& in, cube::Cube& cubeObj, int index){

  if ((int)metricPos.size()<=index) {
    return;
  }
  if (metricPos[index]==-1) {
    return;
  }
  in.clear();
  in.seekg (0, std::ios::end);
//   long int length = in.tellg(); // unused
  in.seekg (metricPos[index], std::ios::beg);  

  int size = SIZE;
  char c[SIZE];
  char c2[100];  
  std::streampos charCounter = 0;
  int pos = size;

  ParseState state = MATRIX;

  int metricId = -1;
  int cnodeId  = -1;
  int threadId = -1;
  double value;
  char* remain;

  std::string str;

  const std::vector<cube::Metric*>& metricVec   = cubeObj.get_metv();
  const std::vector<cube::Cnode*>&  cnodeVec    = cubeObj.get_cnodev();
  const std::vector<cube::Thread*>& threadVec   = cubeObj.get_thrdv();
  cube::Cnode* cnode = NULL;
  cube::Metric* metric = metricVec[index];
  while (true){

    if (pos>=size){
      if (in.eof()) {
	errorStream << "Unexpected EOF\n";
	goto Error;
      }
      in.read(c,size);
      charCounter += size;
      pos = 0;

    }

    //we are in a matrix now

    if (state==MATRIX){

      while (pos<size){
	if (c[pos]==' ' || c[pos]=='\n' || c[pos]=='\t'){
	  ++pos;
	} else if (c[pos]=='<') {
	  ++pos;
	  break;
	} else {
	  errorStream << "\"" << c[pos] << "\": Severity for metric " << metricId << ": " << "Expected </matrix> or <row cnodeid=\"int\">\n";
	  goto Error;
	}
      }
      if (pos>=size && c[pos-1]!='<') continue;	

      if (pos<size) c2[0] =  c[pos++];
      else { assert(!in.eof()); in.get(c2[0]); }

      if (c2[0]=='r') state = MATRIX_ROW;
      else if (c2[0]=='/') state = MATRIX_END;
      else {
	errorStream << "Expected </matrix> or <row cnodeid=\"int\">\n";
	goto Error;
      }
    } 

    //a row starts here

    if (state==MATRIX_ROW){

      for (int i=0; i<12; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else in.get(c2[i]);
      c2[12] = '\0';
      pos += 12;

      if (strcmp(c2,"ow cnodeId=\"")!=0) {
	errorStream << "Expected <row cnodeid=\"\n";
	goto Error;
      }

      int i = 0;
      while (true) {
	if (pos<size) c2[i] = c[pos++];
	else in.get(c2[i]);
	if (c2[i]=='\"'){
	  if (pos<size) c2[i] = c[pos++]; else in.get(c2[i]);
	  if (c2[i]!='>') {
	    errorStream << "Expected int\">\n";	    
	    goto Error;
	  }
	  c2[i] = '\0';
	  break;
	}
	++i;
      }

      cnodeId = (int)strtol(c2,&remain,10); 
      if (remain[0]!='\0') {
	errorStream << "Cnode id should be integer\n";
	goto Error;
      }

      cnode = NULL;
      for (size_t j=0; j<cnodeVec.size(); j++){
	if ((int)(cnodeVec[j]->get_id())==cnodeId){
	  cnode = cnodeVec[j];
	  break;
	}
      }
      if (cnode==NULL){
	errorStream << "Row for undefined cnode found\n";
	goto Error;
      }
      threadId = 0;

      state = ROW;
      if (pos>=size) continue;

    }

    //the matrix ends here

    if (state==MATRIX_END){

      for (int i=0; i<7; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else in.get(c2[i]);
      c2[7] = '\0';
      pos += 7;

      if (strcmp(c2,"matrix>")!=0) {
	errorStream << "Expected </matrix>\n";
	goto Error;
      }
      
      info("");
      errorStream.str("");
      return;
    }

    //we are in a row

    if (state==ROW){

      while (pos<size){

	while (pos<size){
	  if (c[pos]==' ' || c[pos]=='\n' || c[pos]=='\t'){
	    ++pos;
	  } else break;
	}
	if (pos>=size) break;	

	if (c[pos]=='<'){ state = ROW_END; pos++; break;}

	int i = 0;
	while (true) {
	  if (pos<size) c2[i] = c[pos++];
	  else in.get(c2[i]);
	  if (c2[i]=='\n'){
	    c2[i] = '\0';
	    break;
	  }
	  ++i;
	}

	// LOCALE DEPENDEND
	// We espect all data in C Locale
	value = strtod(c2,&remain); 
	if (remain[0]!='\0') {
	  errorStream << "Expected double or </row>\n";
	  goto Error;
	}

	if (value!=0.0){
	  cubeObj.set_sev(metric,
		       cnode,
		       threadVec[threadId++],
		       value);
	}

      }
      if (pos>=size && state==ROW) continue;

    }

    //a row ends here

    if (state==ROW_END){

      for (int i=0; i<5; i++)
	if (pos+i<size) c2[i] = c[pos+i];
	else in.get(c2[i]);
      c2[5] = '\0';
      pos += 5;

      if (strcmp(c2,"/row>")!=0) {
	  errorStream << "Expected </row>\n";
	goto Error;
      }
      state = MATRIX;
      //if (pos>=size) continue;
    }

  }
 Error:
  long int n = (long int)(in.tellg()) - (size-pos-1);
  errorStream << "Syntax error at file position " << n << "\n";
  errorStream << "before: ";
  for (long int i=-20; i<0; i++)
    if (pos+i>=0) errorStream << c[pos+i];
  errorStream << "\nerror: ";
  for (long int i=0; i<=20; i++)
    if (pos+i<size) errorStream << c[pos+i];
  errorStream << "\n";
  error(errorStream.str());

}

void Driver::parse_stream(std::istream& in, cube::Cube& cubeObj)
{

  parseContext->dynamicMetricLoading = false;

  //info("trying cube3 format...\n");

  in.clear();
  in.seekg(0);

  cube3Lexer = new Cube3Scanner(&in, &errorStream, parseContext);
  cube3Parser = new Cube3Parser(*this, *parseContext, *cube3Lexer, cubeObj);

  cube3Parser->parse();

  delete cube3Lexer;
  delete cube3Parser;

  if (parseContext->cubeVersion!=3){

    //info("trying cube2 format...\n");

    in.clear();
    in.seekg(0);
    
    cube2Lexer = new Cube2Scanner(&in, &errorStream, parseContext);
    cube2Parser = new Cube2Parser(*this, *parseContext, *cube2Lexer, cubeObj);
    
    cube2Parser->parse();
    
    delete cube2Parser;
    delete cube2Lexer;
    
  } 

  //info("");
  
}

void Driver::parse_stream_dynamic(std::istream& in, cube::Cube& cubeObj, std::string fileName)
{

  parseContext->dynamicMetricLoading = true;

  in.clear();
  in.seekg(0);

  cube3Lexer = new Cube3Scanner(&in, &errorStream, parseContext);
  cube3Parser = new Cube3Parser(*this, *parseContext, *cube3Lexer, cubeObj);

  cube3Parser->parse();

  delete cube3Lexer;
  delete cube3Parser;

  if (parseContext->cubeVersion!=3){

    info("******** No dynamic loading for cube2 files implemented\n********Loading all data.\n");

    in.clear();
    in.seekg(0);
    
    cube2Lexer = new Cube2Scanner(&in, &errorStream, parseContext);
    cube2Parser = new Cube2Parser(*this, *parseContext, *cube2Lexer, cubeObj);

    cube2Parser->parse();

    delete cube2Parser;
    delete cube2Lexer;
    
  } else {

    int n = cubeObj.get_metv().size();

    in.clear();
    in.seekg (0,std::ios::end);
    long int length = in.tellg();
    in.seekg (0,std::ios::beg);

    std::ifstream infoFile(fileName.c_str());
    
    if (!infoFile) {

      parse_matrix_pos(in);      
      std::ofstream out(fileName.c_str());
      out.precision(30);

      out << length << " ";

      for (int i=0; i<n; i++){
	out << metricPos[i];
	out << " ";
	out << metricSum[i];
	out << " ";
      }
      out.close();      

    } else {

      metricPos.resize(n);
      metricSum.resize(n);
      long int infoLength;
      infoFile >> infoLength;
      if (infoLength!=length)
	error("The length of the cube file is different from\nwhat is stored in the info file!\n\nCould not complete loading.");

      for (int i=0; i<n; i++){
	infoFile >> metricPos[i];
	infoFile >> metricSum[i];
      }
      infoFile.close();
    }

  }

  info("");
}


void Driver::info(const std::string& m)
{
  errorStream << m;
  std::cerr << errorStream.str();
  errorStream.str(""); 
}


void Driver::error_just_message(const std::string& m)
{
  errorStream  << std::endl <<  m << std::endl;
}


void Driver::error(const class location& l,
		   const std::string& m)
{
  errorStream << l << ": " << m << std::endl;
  throw(*(new cube::RuntimeError(errorStream.str())));
}

void Driver::error(const std::string& m)
{
  errorStream << m << std::endl;
  throw(*(new cube::RuntimeError(errorStream.str())));
}

} // namespace cubeparser
