#include "MKCTExtend.h"

MKCTExtend::MKCTExtend(mkc::MKCompressFlag* flag, std::vector<QString>* inputFileList, QString outputFile, QString password)
{
	this->flag = flag;
	this->inputFileList = inputFileList;
	this->outputFile = outputFile;
	this->password = password;
}

void MKCTExtend::run()
{
	mkc::MKCompressResult* compressResult = new mkc::MKCompressResult{ true, errno_t(0) };
	mkc::MKCompressResult* changeHeaderResult = new mkc::MKCompressResult{ true, errno_t(0) };
	
	emit sendMsg(tr("请等待\n"));

	if (flag->forward) {
		if (flag->compress) {
			compressResult = compress();
			if (compressResult->error) {
				emit sendMsg(tr("压缩失败\n"));
				emit sendRunComplate(!compressResult->error, tr(""));
				emit sendMsg(tr("错误代码: compress "));
				emit sendMsg(QString::QString(QChar(compressResult->errValue)));
				emit sendMsg(tr("\n"));
			}
			else {
				emit sendMsg(tr("压缩成功\n"));
			}
		}
		if (flag->changeHeader) {
			changeHeaderResult = changeHeader();
			if (changeHeaderResult->error) {
				emit sendMsg(tr("换头失败\n"));
				emit sendRunComplate(!changeHeaderResult->error, tr(""));
				emit sendMsg(tr("错误代码: changeHeader "));
				emit sendMsg(QString::QString(QChar(changeHeaderResult->errValue)));
				emit sendMsg(tr("\n"));
			}
			else {
				emit sendMsg(tr("换头成功\n"));
			}
		}
	}
	else {
		if (flag->changeHeader) {
			changeHeaderResult = changeHeader();
			if (changeHeaderResult->error) {
				emit sendMsg(tr("还原头失败"));
				emit sendRunComplate(!changeHeaderResult->error, tr(""));
				emit sendMsg(tr("错误代码: changeHeader "));
				emit sendMsg(QString::QString(QChar(changeHeaderResult->errValue)));
				emit sendMsg(tr("\n"));
			}
			else {
				emit sendMsg(tr("还原头成功\n"));
			}
		}
		if (flag->compress) {
			compressResult = compress();
			if (compressResult->error) {
				emit sendMsg(tr("解压失败\n"));
				emit sendRunComplate(!compressResult->error, tr(""));
				emit sendMsg(tr("错误代码: compress "));
				emit sendMsg(QString::QString(QChar(compressResult->errValue)));
				emit sendMsg(tr("\n"));
			}
			else {
				emit sendMsg(tr("解压成功\n"));
			}
		}
	}

	if ((flag->compress && !compressResult->error) || (flag->changeHeader && !changeHeaderResult->error)) {
		emit sendRunComplate(true, tr(""));
	}

	delete compressResult;
	delete changeHeaderResult;
	exec();
}

mkc::MKCompressResult* MKCTExtend::compress()
{
	qDebug() << toString(inputFileList);
	qDebug() << outputFile;
	mkc::MKCompressResult* result = new mkc::MKCompressResult{ true, errno_t(0) };
	return result;
}

mkc::MKCompressResult* MKCTExtend::changeHeader()
{
	qDebug() << toString(inputFileList);
	qDebug() << outputFile;
	mkc::MKCompressResult* result = new mkc::MKCompressResult{ true, errno_t(0) };
	return result;
}

QString MKCTExtend::toString(std::vector<QString>* list)
{
	QString result = tr("----\n");

	for (auto iterator = list->begin(); iterator != list->end(); iterator++) {
		auto element = *iterator;
		result += tr(" ") + element + tr("\n");
	}

	return result;
}
