#pragma once
#pragma comment(lib,"MKCT.lib")

#ifndef MKCTDLL

#include <iostream>
#include <io.h>
#include <cstdio>
#include <stdio.h>

using namespace std;

// define FileErrType
enum class FileErrEnum
{
    NO_ERR,
    OPEN_ERR,
    CLOSE_ERR,
    READ_ERR,
    WRITE_ERR
};

/// <summary>
/// main function: copy head append to end, use MK_HEAD replease head
/// </summary>
/// <param name="file">file path</param>
/// <returns>err type</returns>
extern "C" __declspec(dllimport) FileErrEnum _changeHeaderTo(const string& file);

/// <summary>
/// main function: get last 4 bytes data and replease to first,
/// resize to file size - 4 
/// </summary>
/// <param name="file">file path</param>
/// <returns>error type of program</returns>
extern "C" __declspec(dllimport) FileErrEnum _changeHeaderBack(const string& file);

/// <summary>
/// from string or char* to hex string
/// </summary>
/// <param name="bytes">data</param>
/// <param name="bytelength">length</param>
/// <returns>hex string</returns>
extern "C" __declspec(dllimport) void _bytesToHexString(char* bytes, int bytelength);

#endif // !MKCTDLL
