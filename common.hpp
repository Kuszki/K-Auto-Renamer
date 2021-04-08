#ifndef COMMON_HPP
#define COMMON_HPP

#include <QtCore>

namespace Common
{
	enum class FORMAT
	{
		Base64,
		Hex
	};

	enum class LOG
	{
		DirCreateError,
		MetaCreateError,
		FileCreateError,

		ImageOpenError,

		WrongDpiWarning,
		WrongDocFile
	};

	struct SETTINGS
	{
		int sortBy = 0;
		int nLine = 0;

		int dpi = 300;

		QTextCodec* encoding = QTextCodec::codecForName("UTF-8");
		QCryptographicHash::Algorithm checksum = QCryptographicHash::Sha1;
		FORMAT format = FORMAT::Base64;
		QString text = QString();
	};

	using LOGENTRY = QPair<LOG, QString>;

	using LOGLIST = QList<LOGENTRY>;

	QMap<Common::LOG, QString> LOGMSG(void);
}

#endif // COMMON_HPP
