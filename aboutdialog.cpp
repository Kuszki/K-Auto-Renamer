#include "aboutdialog.hpp"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* Parent) :
	QDialog(Parent),
	ui(new Ui::AboutDialog)
{
	ui->setupUi(this);
}

AboutDialog::~AboutDialog(void)
{
	delete ui;
}
