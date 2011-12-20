/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "LidarProbabilityWidget.h"

// Qt
#include <QIcon>
#include <QFileDialog>
#include <QProgressDialog>

// Called by all constructors
void LidarProbabilityWidget::DefaultConstructor()
{
  std::cout << "DefaultConstructor()" << std::endl;
  this->setupUi(this);

  this->ProgressDialog = new QProgressDialog();
  this->ProgressDialog->setMinimum(0);
  this->ProgressDialog->setMaximum(0);
  this->ProgressDialog->setWindowModality(Qt::WindowModal);
  connect(&this->FutureWatcher, SIGNAL(finished()), this, SLOT(slot_finished()));
  connect(&this->FutureWatcher, SIGNAL(finished()), this->ProgressDialog , SLOT(cancel()));

}

// Default constructor
LidarProbabilityWidget::LidarProbabilityWidget()
{
  std::cout << "PoissonEditingGUI()" << std::endl;
  DefaultConstructor();
};

LidarProbabilityWidget::LidarProbabilityWidget(const std::string& objectFileName, const std::string& scanFileName)
{
  std::cout << "PoissonEditingGUI(string, string)" << std::endl;
  DefaultConstructor();
  this->ObjectFileName = objectFileName;
  this->ScanFileName = scanFileName;

}

void LidarProbabilityWidget::showEvent ( QShowEvent * event )
{

}

void LidarProbabilityWidget::resizeEvent ( QResizeEvent * event )
{

}

void LidarProbabilityWidget::on_actionSaveResult_activated()
{
  // Get a filename to save
  QString fileName = QFileDialog::getSaveFileName(this, "Save File", ".", "Image Files (*.jpg *.jpeg *.bmp *.png *.mha)");

  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }


  this->statusBar()->showMessage("Saved result.");
}

// void LidarProbabilityWidget::OpenImageAndMask(const std::string& imageFileName, const std::string& maskFileName)
// {
// 
// }

void LidarProbabilityWidget::on_actionOpenImage_activated()
{
  std::cout << "on_actionOpenImage_activated" << std::endl;

}

void LidarProbabilityWidget::slot_StartProgressBar()
{
  this->progressBar->show();
}

void LidarProbabilityWidget::slot_StopProgressBar()
{
  this->progressBar->hide();
}

void LidarProbabilityWidget::slot_IterationComplete()
{
  //QFuture<void> future = QtConcurrent::run(&this->MyObject, &MyClass::LongFunction);
  //this->FutureWatcher.setFuture(future);

  this->ProgressDialog->exec();
}
