#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	Worker = new ThreadWorker();
	Worker->moveToThread(&Thread);
	Worker->setSettings(SettingsDialog::loadSettings());
	Thread.start();

	Terminator = new QPushButton(QIcon::fromTheme("process-stop"), tr("Stop"), this);
	Terminator->hide();

	Progress = new QProgressBar(this);
	Progress->hide();

	ui->statusbar->addPermanentWidget(Progress);
	ui->statusbar->addPermanentWidget(Terminator);

	QSettings Settings("Scancentre", "Katalog");

	Settings.beginGroup("Window");
	restoreGeometry(Settings.value("geometry").toByteArray());
	restoreState(Settings.value("state").toByteArray());
	srcState = Settings.value("srchead").toByteArray();
	dstState = Settings.value("dsthead").toByteArray();
	Settings.endGroup();

	Settings.beginGroup("Paths");
	ui->srcEdit->setText(Settings.value("source").toString());
	ui->dstEdit->setText(Settings.value("dest").toString());
	Settings.endGroup();

	Settings.beginGroup("View");
	ui->actionDbclick->setChecked(Settings.value("open", false).toBool());
	Settings.endGroup();

	if (!ui->srcEdit->text().isEmpty())
		sourcePathChanged(ui->srcEdit->text());

	if (!ui->dstEdit->text().isEmpty())
		destPathChanged(ui->dstEdit->text());

	connect(ui->actionExit, &QAction::triggered,
		   this, &MainWindow::close);

	connect(ui->actionProceed, &QAction::triggered,
		   this, &MainWindow::startActionClicked);

	connect(ui->actionPreferences, &QAction::triggered,
		   this, &MainWindow::settingsActionClicked);

	connect(ui->actionAbout, &QAction::triggered,
		   this, &MainWindow::aboutActionClicked);

	connect(ui->srcButton, &QToolButton::clicked,
		   this, &MainWindow::openSourceClicked);

	connect(ui->dstButton, &QToolButton::clicked,
		   this, &MainWindow::openDestClicked);

	connect(ui->srcEdit, &QLineEdit::textChanged,
		   this, &MainWindow::sourcePathChanged);

	connect(ui->dstEdit, &QLineEdit::textChanged,
		   this, &MainWindow::destPathChanged);

	connect(ui->srcEdit, &QLineEdit::textChanged,
		   this, &MainWindow::dataPathsChanged);

	connect(ui->dstEdit, &QLineEdit::textChanged,
		   this, &MainWindow::dataPathsChanged);

	connect(ui->srcView, &QTreeView::doubleClicked,
		   this, &MainWindow::treeItemDbclicked);

	connect(ui->dstView, &QTreeView::doubleClicked,
		   this, &MainWindow::treeItemDbclicked);

	connect(Worker, &ThreadWorker::onJobStart,
		   Progress, &QProgressBar::setRange);

	connect(Worker, &ThreadWorker::onJobProgress,
		   Progress, &QProgressBar::setValue);

	connect(Worker, &ThreadWorker::onJobDone,
		   this, &MainWindow::workerJobDone);

	connect(this, &MainWindow::onJobRequest,
		   Worker, &ThreadWorker::startJob);

	connect(Terminator, &QPushButton::clicked,
		   Terminator, &QPushButton::hide);

	connect(Terminator, &QPushButton::clicked,
		   Worker, &ThreadWorker::calcelJob,
		   Qt::DirectConnection);

	dataPathsChanged();
}

MainWindow::~MainWindow(void)
{
	if (dynamic_cast<QFileSystemModel*>(ui->srcView->model()))
		srcState = ui->srcView->header()->saveState();

	if (dynamic_cast<QFileSystemModel*>(ui->dstView->model()))
		dstState = ui->dstView->header()->saveState();

	QSettings Settings("Scancentre", "Katalog");

	Settings.beginGroup("Window");
	Settings.setValue("state", saveState());
	Settings.setValue("geometry", saveGeometry());
	Settings.setValue("srchead", srcState);
	Settings.setValue("dsthead", dstState);
	Settings.endGroup();

	Settings.beginGroup("View");
	Settings.setValue("open", ui->actionDbclick->isChecked());
	Settings.endGroup();

	Thread.exit();
	Thread.wait();

	delete ui;
	delete Worker;
}

void MainWindow::startActionClicked(void)
{
	if (!Worker->isStartable()) return;

	const auto src = ui->srcEdit->text();
	const auto dst = ui->dstEdit->text();

	if (!QDir().exists(src))
		ui->statusbar->showMessage(tr("Source path doesn't exists"));
	else if (!QDir().exists(dst))
		ui->statusbar->showMessage(tr("Destonation path doesn't exists"));

	if (!QDir().exists(src) || !QDir().exists(dst)) return;

	Progress->show();
	Terminator->show();

	ui->actionProceed->setEnabled(false);
	ui->actionPreferences->setEnabled(false);
	ui->actionExit->setEnabled(false);

	ui->dstBox->setEnabled(false);
	ui->srcBox->setEnabled(false);

	emit onJobRequest(src, dst);
}

void MainWindow::settingsActionClicked(void)
{
	SettingsDialog* Dialog = new SettingsDialog(this);

	connect(Dialog, &SettingsDialog::onAccepted,
		   Worker, &ThreadWorker::setSettings);

	connect(Dialog, &SettingsDialog::accepted,
		   Dialog, &SettingsDialog::deleteLater);

	connect(Dialog, &SettingsDialog::rejected,
		   Dialog, &SettingsDialog::deleteLater);

	Dialog->open();
}

void MainWindow::aboutActionClicked(void)
{
	AboutDialog* Dialog = new AboutDialog(this);

	connect(Dialog, &AboutDialog::accepted,
		   Dialog, &AboutDialog::deleteLater);

	connect(Dialog, &AboutDialog::rejected,
		   Dialog, &AboutDialog::deleteLater);

	Dialog->open();
}

void MainWindow::openSourceClicked(void)
{
	QSettings Settings("Scancentre", "Katalog");

	Settings.beginGroup("Paths");

	const QString lastDir = Settings.value("source", QString()).toString();

	const QString Path = QFileDialog::getExistingDirectory(this,
		tr("Select source directory"), lastDir);

	if (!Path.isEmpty())
	{
		ui->srcEdit->setText(Path);
		Settings.setValue("source", Path);
	}
}

void MainWindow::openDestClicked(void)
{
	QSettings Settings("Scancentre", "Katalog");
	Settings.beginGroup("Paths");

	const QString lastDir = Settings.value("dest", QString()).toString();

	const QString Path = QFileDialog::getExistingDirectory(this,
		tr("Select destonation directory"), lastDir);

	if (!Path.isEmpty())
	{
		ui->dstEdit->setText(Path);
		Settings.setValue("dest", Path);
	}
}

void MainWindow::sourcePathChanged(const QString& Path)
{
	auto lastModel = ui->srcView->model();
	if (lastModel) lastModel->deleteLater();

	if (dynamic_cast<QFileSystemModel*>(lastModel))
		srcState = ui->srcView->header()->saveState();

	auto model = new QFileSystemModel(this);
	model->setRootPath(Path);

	ui->srcView->setModel(model);
	ui->srcView->setRootIndex(model->index(Path));
	ui->srcView->header()->restoreState(srcState);
}

void MainWindow::destPathChanged(const QString& Path)
{
	auto lastModel = ui->dstView->model();
	if (lastModel) lastModel->deleteLater();

	if (dynamic_cast<QFileSystemModel*>(lastModel))
		dstState = ui->dstView->header()->saveState();

	if (!QDir(Path).exists()) return;

	auto model = new QFileSystemModel(this);
	model->setRootPath(Path);

	ui->dstView->setModel(model);
	ui->dstView->setRootIndex(model->index(Path));
	ui->dstView->header()->restoreState(dstState);
}

void MainWindow::dataPathsChanged(void)
{
	const auto src = ui->srcEdit->text().replace('\\', '/');
	const auto dst = ui->dstEdit->text().replace('\\', '/');

	if (src.isEmpty())
		ui->statusbar->showMessage(tr("Select source path"));
	else if (dst.isEmpty())
		ui->statusbar->showMessage(tr("Select destonation path"));
	else if (src == dst)
		ui->statusbar->showMessage(tr("Source and destonation paths are the same"));
	else if ((dst + '/').contains(src + '/'))
		ui->statusbar->showMessage(tr("Source path contains destonation path"));
	else if (!QDir().exists(src))
		ui->statusbar->showMessage(tr("Source path doesn't exists"));
	else if (!QDir().exists(dst))
		ui->statusbar->showMessage(tr("Destonation path doesn't exists"));

	const bool ok = (src != dst) && !((dst + '/').contains(src + '/')) &&
				 !src.isEmpty() && !dst.isEmpty() &&
				 QDir().exists(src) && QDir().exists(dst);

	ui->actionProceed->setEnabled(ok);
}

void MainWindow::workerJobDone(const Common::LOGLIST& Log)
{
	LogsDialog* Dialog = new LogsDialog(this);

	connect(Dialog, &LogsDialog::accepted,
		   Dialog, &LogsDialog::deleteLater);

	connect(Dialog, &LogsDialog::rejected,
		   Dialog, &LogsDialog::deleteLater);

	Dialog->showLogs(Log);
	Dialog->open();

	Progress->hide();
	Terminator->hide();

	ui->actionProceed->setEnabled(true);
	ui->actionPreferences->setEnabled(true);
	ui->actionExit->setEnabled(true);

	ui->dstBox->setEnabled(true);
	ui->srcBox->setEnabled(true);
}

void MainWindow::treeItemDbclicked(const QModelIndex& Index)
{
	if (!ui->actionDbclick->isChecked()) return;

	const auto view = dynamic_cast<QAbstractItemView*>(sender());
	if (!view) return;

	const auto model = dynamic_cast<QFileSystemModel*>(view->model());
	if (!model) return;

	QDesktopServices::openUrl(QUrl::fromLocalFile(model->filePath(Index)));
}
