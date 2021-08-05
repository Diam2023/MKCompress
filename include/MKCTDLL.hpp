#pragma once
#pragma comment(lib,"MKCT.lib")

#ifndef MKCTDLL

/*
* author: monoliths
* date: 2021 6 19
* version: 1.1
*/

/// <summary>
/// main function: copy head append to end, use MK_HEAD replease head
/// </summary>
/// <param name="file">file path</param>
/// <returns>err type</returns>
extern "C" __declspec(dllimport) errno_t _changeHeaderTo(const wchar_t* file);

/// <summary>
/// main function: get last 4 bytes data and replease to first,
/// resize to file size - 4 
/// </summary>
/// <param name="file">file path</param>
/// <returns>error type of program</returns>
extern "C" __declspec(dllimport) errno_t _changeHeaderBack(const wchar_t* file);

/// <summary>
/// get file head
/// </summary>
/// <param name="file">file path</param>
/// <returns>a hex head</returns>
extern "C" __declspec(dllimport) char* _getFileHeader(const wchar_t* file);

/// <summary>
/// get mkc file origin head
/// </summary>
/// <param name="file"></param>
/// <returns></returns>
extern "C" __declspec(dllimport) char* _getMKFileHeader(const wchar_t* file);

/// <summary>
/// from string or char* to hex string
/// </summary>
/// <param name="bytes">data</param>
/// <param name="bytelength">length</param>
extern "C" __declspec(dllimport) void _bytesToHexString(char* bytes, int bytelength);
/// <summary>
/// add bytes to last
/// only support save over length of 4 bytes to exprence bytes
/// </summary>
/// <param name="bytes">data</param>
extern "C" __declspec(dllimport) errno_t _pushLastBytes(const wchar_t* file, const char* bytes);

/// <summary>
/// return and delete the last of added bytes
/// </summary>
/// <returns></returns>
extern "C" __declspec(dllimport) char* _popLatsBytes(const wchar_t* file);

#endif // !MKCTDLL
