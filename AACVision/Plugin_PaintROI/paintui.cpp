#include "paintui.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>


PaintUI::PaintUI(QWidget *parent)
	: QMainWindow(parent), myview(nullptr)
{
	ui.setupUi(this);
    setAttribute(Qt::WA_QuitOnClose, false);
	auto layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
    myscene = new MyScene(this);
	myview = new MyView(nullptr, myscene);
    ui.verticalLayout_2->addWidget(myview);
    connect(ui.pb_open, &QPushButton::clicked, this, &PaintUI::slot_pb_open);
    connect(ui.pb_magnify, &QPushButton::clicked, this, [=] {
        myview->zoom(QPoint(0,1));
        });
    connect(ui.pb_shrink, &QPushButton::clicked, this, [=] {
        myview->zoom(QPoint(0, -1));
        });
    connect(ui.pb_reposition, &QPushButton::clicked, this, [=] {
        myview->reset();
        });
    connect(ui.pb_clear, &QPushButton::clicked, this, [=] {
        myview->clear();
        });
    connect(ui.pb_redraw, &QPushButton::clicked, this, [=] {
        for (auto item:myscene->items() )
        {
            if (item->type() > QGraphicsItem::UserType) {
                myscene->removeItem(item);
            }
        }
        });
    connect(ui.pb_save, &QPushButton::clicked, this, [=] {
        for (auto item : myscene->items())
        {
            if (item->type() > QGraphicsItem::UserType) {
                auto myItem = static_cast<DrawItemAbstract*>(item);
                if (!myItem->save())
                {
                    QMessageBox::information(this, u8"信息", QString(u8"%1保存失败").arg(myItem->name()), QMessageBox::Ok, QMessageBox::Ok);
                    return;
                }
            }
        }
        });
    QVBoxLayout* vlayout = new QVBoxLayout;
    ui.scrollArea->widget()->setLayout(vlayout);
    vlayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    auto fac = myscene->getItemFactorys();
    QMenu* menu = new QMenu(u8"加载ROI");
    for (auto& str : myscene->getItemClasses())
    {
        QPushButton* btn = new QPushButton(str);
        connect(btn, &QPushButton::clicked, this, [=] {
            myscene->setDrawItemClass(str);
            });
        vlayout->addWidget(btn);

        QAction* act = new QAction(str);
        connect(act, &QAction::triggered, this, [=] {
            QStringList files = QFileDialog::getOpenFileNames(this, u8"打开ROI文件", "./", "ROI (*.dxf *.hobj)");
            for (auto& f : files) {
                auto item = fac.value(str)->createItem(f);
                if (item==nullptr)
                {
                    QMessageBox::critical(nullptr, QString(u8"%1错误").arg(str), QString(u8"%1加载失败").arg(f), QMessageBox::Ok, QMessageBox::Ok);
                }
                else
                {
                    myscene->addItem(item);
                }
            }
            });
        menu->addAction(act);
    }
    vlayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui.menuBar->addMenu(menu);
    
    auto mainStatusBar = statusBar();

    pathInfo = new QLabel(mainStatusBar);
    mainStatusBar->addWidget(pathInfo);
    pathInfo->setText(u8"路径：");

    stateInfo = new QLabel(mainStatusBar);
    mainStatusBar->addWidget(stateInfo);
    stateInfo->setText(u8"状态：");

    corInfo = new QLabel(mainStatusBar);
    corInfo->setText(u8"R： C： ");
    mainStatusBar->addPermanentWidget(corInfo);

    pixValue = new QLabel(mainStatusBar);
    pixValue->setText(u8"像素值：");
    mainStatusBar->addPermanentWidget(pixValue);
    
    
}

PaintUI::~PaintUI()
{}

void PaintUI::slot_pb_open()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle(u8"打开图片");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg *.jpeg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        myview->setImage(filePaths.at(0));
        pathInfo->setText(QString(u8"路径：%1").arg(filePaths.at(0)));
    }
}
