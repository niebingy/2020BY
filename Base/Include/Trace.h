/*******************************************************************
** �ļ���:	Trace.h
** ��  ��:
** Ӧ  ��:	���ں�����־ϵͳ
********************************************************************/
#pragma once
#include <string>
#include <sstream>
#include <iostream> 
using namespace std;
#define TraceLn(m, x)		 \
	ostringstream oss; oss << x; std::cout <<"[TRACE] "<< oss.str() << std::endl;


#define WarningLn(m, x)     \
	ostringstream oss; oss << x; std::cout << "[WARNING] " << oss.str() << std::endl;

#define ErrorLn(m, x)     \
	ostringstream oss; oss << x; std::cout << "[ERROR] " << oss.str() << std::endl;