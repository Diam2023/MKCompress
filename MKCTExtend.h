#pragma once

#ifndef MKCTExtend

#include <qthread.h>
#include "stdafx.h"
#include "MKCompress.h"

class MKCTExtend :
    public QThread
{
    Q_OBJECT
private:
	mkc::MKCompressFlag* flag;
	std::vector<QString>* inputFileList;
	QString inputFile;
	QString outputFile;
	QString password;

public:
    /// <summary>
    /// Run main envarment
    /// </summary>
    /// <param name="flag">flag of run</param>
	/// <param name="inputFileList">Input files path</param>
	/// <param name="outputFile">Output file path</param>
	/// <param name="password">Use password to compress</param>
	MKCTExtend(mkc::MKCompressFlag* flag, std::vector<QString>* inputFileList, QString outputFile, QString password);

    /// <summary>
    /// extren to QThread <br>
	/// main run
    /// </summary>
    void run();

	/// <summary>
	/// To compress or decompress file
	/// </summary>
	/// <returns>error flag</returns>
	mkc::MKCompressResult* compress();

	/// <summary>
	/// To change file header to or back
	/// </summary>
	/// <returns>error flag</returns>
	mkc::MKCompressResult* changeHeader();

	/// <summary>
	/// to string
	/// </summary>
	/// <param name="list">input data list</param>
	/// <returns>file list data</returns>
	QString toString(std::vector<QString>* list);

signals:
    /// <summary>
    /// send message to output content
    /// </summary>
    void sendMsg(QString msg);
	
	/// <summary>
	/// flag to announcement
	/// </summary>
	/// <param name="flag">if true is complate</param>
	/// <param name="msg">output message</param>
	void sendRunComplete(bool flag, QString msg);
};

#endif // !MKCTExtend