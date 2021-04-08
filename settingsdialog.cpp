#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

const QVector<QCryptographicHash::Algorithm> SettingsDialog::checksums =
{
	QCryptographicHash::Md4,
	QCryptographicHash::Md5,
	QCryptographicHash::Sha1,
	QCryptographicHash::Sha224,
	QCryptographicHash::Sha256,
	QCryptographicHash::Sha384,
	QCryptographicHash::Sha512,
	QCryptographicHash::Sha3_224,
	QCryptographicHash::Sha3_256,
	QCryptographicHash::Sha3_384,
	QCryptographicHash::Sha3_512,
	QCryptographicHash::Keccak_224,
	QCryptographicHash::Keccak_256,
	QCryptographicHash::Keccak_384,
	QCryptographicHash::Keccak_512
};

const QVector<Common::FORMAT> SettingsDialog::formats =
{
	Common::FORMAT::Base64,
	Common::FORMAT::Hex
};

SettingsDialog::SettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);

	QSettings Settings("Scancentre", "Katalog");

	auto Expr = QRegularExpression("^[\\w\\-.]*$");
	auto Validator = new QRegularExpressionValidator(Expr, this);

	ui->textEdit->setValidator(Validator);

	Settings.beginGroup("Settings");
	ui->sortCombo->setCurrentIndex(Settings.value("sort", 0).toInt());
	ui->nlineCombo->setCurrentIndex(Settings.value("nline", 0).toInt());
	ui->encodingCombo->setCurrentText(Settings.value("encoding", "UTF-8").toString());
	ui->dpiSpin->setValue(Settings.value("dpi", 300).toInt());
	ui->checksumCombo->setCurrentIndex(Settings.value("checksum", 2).toInt());
	ui->formatCombo->setCurrentIndex(Settings.value("format", 0).toInt());
	ui->textEdit->setText(Settings.value("text", QString()).toString());
	Settings.endGroup();

	connect(ui->encodingCombo, &QComboBox::currentTextChanged,
		   this, &SettingsDialog::validate);

	connect(ui->buttonBox->button(QDialogButtonBox::Save), &QPushButton::clicked,
		   this, &SettingsDialog::accept);

	connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked,
		   this, &SettingsDialog::reset);

	connect(ui->buttonBox->button(QDialogButtonBox::Discard), &QPushButton::clicked,
		   this, &SettingsDialog::reject);
}

SettingsDialog::~SettingsDialog(void)
{
	delete ui;
}

Common::SETTINGS SettingsDialog::settings(void) const
{
	return
	{
		ui->sortCombo->currentIndex(),
		ui->nlineCombo->currentIndex(),

		ui->dpiSpin->value(),

		QTextCodec::codecForName(ui->encodingCombo->currentText().toUtf8()),

		checksums.value(ui->checksumCombo->currentIndex(), QCryptographicHash::Sha1),
		formats.value(ui->formatCombo->currentIndex(), Common::FORMAT::Base64),

		ui->textEdit->text()
	};
}

Common::SETTINGS SettingsDialog::loadSettings(void)
{
	QSettings Settings("Scancentre", "Katalog");

	Settings.beginGroup("Settings");

	return
	{
		Settings.value("sort", 0).toInt(),
		Settings.value("nline", 0).toInt(),

		Settings.value("dpi", 300).toInt(),

		QTextCodec::codecForName(Settings.value("encoding", "UTF-8").toString().toUtf8()),

		checksums.value(Settings.value("checksum", 2).toInt(), QCryptographicHash::Sha1),
		formats.value(Settings.value("format", 0).toInt(), Common::FORMAT::Base64),

		Settings.value("text", QString()).toString()
	};
}

void SettingsDialog::accept(void)
{
	QSettings Settings("Scancentre", "Katalog");

	Settings.beginGroup("Settings");
	Settings.setValue("sort", ui->sortCombo->currentIndex());
	Settings.setValue("nline", ui->nlineCombo->currentIndex());
	Settings.setValue("encoding", ui->encodingCombo->currentText());
	Settings.setValue("dpi", ui->dpiSpin->value());
	Settings.setValue("checksum", ui->checksumCombo->currentIndex());
	Settings.setValue("format", ui->formatCombo->currentIndex());
	Settings.setValue("text", ui->textEdit->text());
	Settings.endGroup();

	emit onAccepted(
	{
		ui->sortCombo->currentIndex(),
		ui->nlineCombo->currentIndex(),

		ui->dpiSpin->value(),

		QTextCodec::codecForName(ui->encodingCombo->currentText().toUtf8()),

		checksums.value(ui->checksumCombo->currentIndex(), QCryptographicHash::Sha1),
		formats.value(ui->formatCombo->currentIndex(), Common::FORMAT::Base64),

		ui->textEdit->text()
	});

	QDialog::accept();
}

void SettingsDialog::reset(void)
{
	ui->sortCombo->setCurrentIndex(0);
	ui->nlineCombo->setCurrentIndex(0);
	ui->encodingCombo->setCurrentText("UTF-8");
	ui->dpiSpin->setValue(300);
	ui->checksumCombo->setCurrentText("SHA-1");
	ui->formatCombo->setCurrentText("Base64");
	ui->textEdit->setText(QString());
}

void SettingsDialog::validate(void)
{
	const auto codec = QTextCodec::codecForName(ui->encodingCombo->currentText().toUtf8());

	ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(codec);
}
