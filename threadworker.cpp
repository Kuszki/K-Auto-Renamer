#include "threadworker.hpp"

ThreadWorker::ThreadWorker(QObject *parent)
	: QObject(parent)
{

}

ThreadWorker::~ThreadWorker(void)
{

}

bool ThreadWorker::isCanceled(void) const
{
	QMutexLocker Locker(&Mutex);

	return Job && Kill;
}

bool ThreadWorker::isStartable(void) const
{
	QMutexLocker Locker(&Mutex);

	return !Job && !Kill;
}

void ThreadWorker::start(void)
{
	QMutexLocker Locker(&Mutex);

	Kill = false;
	Job = true;
}

void ThreadWorker::stop(void)
{
	QMutexLocker Locker(&Mutex);

	Kill = false;
	Job = false;
}

void ThreadWorker::setSettings(const Common::SETTINGS Settings)
{
	if (Job) return;

	Prefs.dpi = Settings.dpi;

	Prefs.codec = Settings.encoding;
	Prefs.checksum = Settings.checksum;

	switch (Settings.sortBy)
	{
		case 0:
			Prefs.sort = QDir::Name;
		break;
		case 1:
			Prefs.sort = QDir::Time;
		break;
		default:
			Prefs.sort = QDir::LocaleAware;
	}

	switch (Settings.nLine)
	{
		default:
		case 0:
			Prefs.endl = "\n";
		break;
		case 1:
			Prefs.endl = "\r\n";
		break;
		case 2:
			Prefs.endl = "\r";
		break;
	}

	switch (Settings.format)
	{
		case Common::FORMAT::Base64:
			Prefs.format = QOverload<>::of(&QByteArray::toBase64);
		break;
		case Common::FORMAT::Hex:
			Prefs.format = QOverload<>::of(&QByteArray::toHex);
		break;
	}

	if (!Settings.text.isEmpty())
		Prefs.text = Settings.text + '_';
}

void ThreadWorker::startJob(const QString& src, const QString& dst)
{
	static const auto dirFilter = QDir::Dirs | QDir::NoDotAndDotDot;

	if (isStartable()) start(); else return;
	int Step(0);

	emit onJobTitle(tr("Calculating directory count"));
	emit onJobStart(0, 0);

	QStringList Tasks;
	QMutex Synchronizer;
	Common::LOGLIST Logs;

	for (const auto& year : QDir(src).entryList(dirFilter, Prefs.sort))
		for (const auto& number : QDir(src + '/' + year).entryList(dirFilter, Prefs.sort))
	   {
		  Tasks.append(year + '/' + number);
		  QDir(dst).mkdir(year);
	   }

	emit onJobTitle(tr("Processing directories"));
	emit onJobStart(0, Tasks.length());

	QtConcurrent::blockingMap(Tasks, [this, &src, &dst, &Synchronizer, &Logs, &Step] (const QString& dirname) -> void
	{
		if (this->isCanceled()) return;

		static const auto otherFilter = QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot;
		static const auto fileFilter = QDir::Files | QDir::NoDotAndDotDot;
		static const auto formatFilter = QStringList({ "*.jpg", "*.jpeg" });

		const auto meta = dirname.split('/');
		const auto year = meta.value(0);
		const auto number = meta.value(1);

		const auto files = QDir(src + '/' + dirname).entryList(formatFilter, fileFilter, Prefs.sort);
		const auto others = QDir(src + '/' + dirname).entryList(QStringList(), otherFilter);

		if (!QDir(dst).mkpath(dirname))
		{
			QMutexLocker Locker(&Synchronizer);

			Logs.append(qMakePair(Common::LOG::DirCreateError, dst + '/' + dirname));

			emit onJobProgress(++Step); return;
		}

		QFile Metadata(dst + '/' + dirname + "/operat.txt");
		QTextStream Stream(&Metadata);
		Common::LOGLIST currLogs;

		if (!Metadata.open(QFile::WriteOnly | QFile::Text))
		{
			currLogs.append(qMakePair(Common::LOG::MetaCreateError, Metadata.fileName()));
		}
		else
		{
			Stream << number << ' ' << year << ' ' << files.length() << Prefs.endl;

			int page(0); for (const auto& file : files)
			{
				const QString srcpath = src + '/' +
								    dirname + '/' +
								    file;

				const QString pnum = QString::number(++page).rightJustified(3, '0');

				const QString filename = number + '_' +
									year + '_' +
									Prefs.text +
									pnum + ".jpg";

				const QString dstpath = dst + '/' +
								    dirname + '/' +
								    filename;

				const QImage Image(srcpath);
				QFile Imagefile(srcpath);

				if (!Imagefile.open(QFile::ReadOnly))
				{
					currLogs.append(qMakePair(Common::LOG::ImageOpenError, srcpath));
				}
				else
				{
					const int res = qRound((2.54 / 200.0) *
									   (Image.dotsPerMeterX() +
									    Image.dotsPerMeterX()));

					if (qAbs(res - Prefs.dpi) >= 5)
					{
						currLogs.append(qMakePair(Common::LOG::WrongDpiWarning, srcpath));
					}

					const auto array = QCryptographicHash::hash(Imagefile.readAll(), Prefs.checksum);
					const auto hash = (array.*Prefs.format)();

					Stream << page << ' ' << res << ' ' << hash << Prefs.endl;

					if (QFile::exists(dstpath)) QFile::remove(dstpath);

					if (!QFile::copy(srcpath, dstpath))
					{
						currLogs.append(qMakePair(Common::LOG::FileCreateError, dstpath));
					}
				}

			}
		}

		for (const auto& fname : others) if (!files.contains(fname))
		{
			const QString srcpath = src + '/' +
							    dirname + '/' +
							    fname;

			currLogs.append(qMakePair(Common::LOG::WrongDocFile, srcpath));
		}

		if (!currLogs.isEmpty())
		{
			const QMap<Common::LOG, QString> Messages = Common::LOGMSG();

			QFile warnFile(dst + '/' + dirname + "/uwagi.txt");
			QTextStream warnStream(&warnFile);

			if (warnFile.open(QFile::WriteOnly | QFile::Text))
				for (const auto& Log : currLogs)
				{
					warnStream << QString("%1:\t%2%3")
						.arg(Messages[Log.first])
						.arg(Log.second)
						.arg(Prefs.endl);
				}
		}

		Synchronizer.lock();
		emit onJobProgress(++Step);
		Logs.append(currLogs);
		Synchronizer.unlock();
	});

	stop(); emit onJobDone(Logs);
}

void ThreadWorker::calcelJob(void)
{
	QMutexLocker Locker(&Mutex);

	if (Job) Kill = true;
}
