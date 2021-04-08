#include "logsdialog.hpp"
#include "ui_logsdialog.h"

LogsDialog::LogsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::LogsDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked,
		   this, &LogsDialog::saveButtonClicked);

	connect(ui->buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked,
		   this, &LogsDialog::reject);

	connect(ui->textBrowser, &QTextBrowser::anchorClicked, &QDesktopServices::openUrl);
}

LogsDialog::~LogsDialog(void)
{
	delete ui;
}

void LogsDialog::showLogs(const Common::LOGLIST& Logs)
{
	if (Logs.isEmpty()) { ui->textBrowser->setPlainText(tr("No warnings")); return; }

	const QMap<Common::LOG, QString> Messages = Common::LOGMSG();

	QString Lines; for (const auto& Log : Logs) Lines.append(
		QString("%1:\t<a href=\"%3\">%2</a><br>\n")
		.arg(Messages[Log.first]).arg(Log.second)
		.arg(QUrl::fromLocalFile(Log.second).toString()));

	ui->textBrowser->setHtml(Lines);
}

bool LogsDialog::saveLogs(const QString& Path)
{
	QFile File(Path);
	QTextStream Stream(&File);

	if (!File.open(QFile::WriteOnly | QFile::Text)) return false;

	Stream << ui->textBrowser->toPlainText();

	return true;
}

void LogsDialog::saveButtonClicked(void)
{
	const QString Path = QFileDialog::getSaveFileName(this,
		tr("Select file to save logs"), "~/", tr("Text files (*.txt);;Log files (*.log)"));

	if (!Path.isEmpty() && !saveLogs(Path)) QMessageBox::critical(this,
		tr("Error"), tr("Error in saving log file"));
}
