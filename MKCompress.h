#pragma once

#if defined(UNICODE) && defined(_UNICODE)
#pragma message("UNICODE and _UNICODE macro activated.")
#else
#error UNICODE and _UNICODE must be defined. This version only supports unicode builds.
#endif // !UNICONDE

#include <QtWidgets/QMainWindow>
#include "ui_MKCompress.h"
#include "stdafx.h"

namespace mkc {

	// To save soft run mode
	enum class COMPRESS_MODE {
		DEFAULT,
		ONLY_CHANGE_HEADER,
		ONLY_COMPRESS,
		COMPRESS_AND_CHANGE_HEADER
	};

	const QString MKC_TYPE = ".mkc";

	const QString SEVENZ_TYPE = ".7z";

	// MKC type header
	const std::byte MKC_HEAD[4]{ std::byte{0x20}, std::byte{0x15}, std::byte{0x12}, std::byte{0x27} };

	// 7z header (only use 4 byte)
	const std::byte SEVENZ_HEAD[4]{ std::byte{0x37}, std::byte{0x7a}, std::byte{0xbc}, std::byte{0xaf} };

	// The flag to solve check boxs status
	// To show is able check
	struct MKCompressFlag
	{
		// if forward is to compress or change header
		bool forward;

		bool changeHeaderAble;

		bool compressAble;

		bool changeHeader;
		bool compress;

		mkc::COMPRESS_MODE mode;
	};

	// to change byte[] to char* type
	constexpr auto stdBytesToChars = [&](const std::byte bytes[]) {
		char* data = (char*)malloc(sizeof(char) * 4);
		if (data != nullptr) {
			for (int i = 0; i < 4; i++)
			{
				data[i] = std::to_integer<char>(bytes[i]);
			}
		}
		return data;
	};
}

class MKCompress : public QMainWindow
{
	Q_OBJECT

public:
	MKCompress(QWidget* parent = Q_NULLPTR);
	~MKCompress();

private:
	Ui::MKCompressClass ui;

	char* MKC_HEADER;
	char* SEVENZ_HEADER;

	// launch flag
	mkc::MKCompressFlag* launchFlag = new mkc::MKCompressFlag{ true, false, false, false, false, mkc::COMPRESS_MODE::DEFAULT };

	// unique_ptr fileListData;
	std::unique_ptr<std::vector<QString>, std::default_delete<std::vector<QString>>> fileListData = std::make_unique<std::vector<QString>>();

	QString password;

	// output launch flag
	void outputFlag();

	/// <summary>
	/// Main initialize
	/// </summary>
	void init();

	/// <summary>
	/// Flush fileListData to fileListView
	/// </summary>
	void flushData();

	/// <summary>
	/// To compress or decompress file
	/// </summary>
	/// <param name="flag">To compress if flag is true,Or decompress</param>
	/// <param name="inputFile">Input file path</param>
	/// <param name="outputFile">Output file path</param>
	/// <param name="pwd">password</param>
	void launchCompress(bool flag, std::unique_ptr<std::vector<QString>, std::default_delete<std::vector<QString>>>& inputFileList, QString outputFile, QString pwd);

	/// <summary>
	/// To change file header to or back
	/// </summary>
	/// <param name="flag">If is true it will be change header to;else it will change back</param>
	/// <param name="path">File path</param>
	void changeHeader(bool flag, QString path);
	/*
	char* getCharsMKC_HEAD();
	char* getCharsSEVENZ_HEAD();
	*/

	void flushCheckBoxStatus();

	void flushLaunchStatus();

	void flushOutputPath();

private slots:
	void openDialog();
	void custumContextMenu(const QPoint& pos);
	void deleteSeedSlot();
	void clearSeedsSlot();
};
