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

#ifndef LidarProbabilityWidget_H
#define LidarProbabilityWidget_H

#include "ui_LidarProbabilityWidget.h"

// Qt
#include <QMainWindow>
#include <QFutureWatcher>

class QProgressDialog;

class LidarProbabilityWidget : public QMainWindow, public Ui::LidarProbabilityWidget
{
  Q_OBJECT
public:
  void DefaultConstructor();
  LidarProbabilityWidget();
  LidarProbabilityWidget(const std::string& objectFileName, const std::string& scanFileName);

public slots:

  void on_actionOpenImage_activated();
  void on_actionSaveResult_activated();

  void slot_StartProgressBar();
  void slot_StopProgressBar();
  void slot_IterationComplete();

protected:

  void showEvent ( QShowEvent * event );
  void resizeEvent ( QResizeEvent * event );

  //void OpenImageAndMask(const std::string& imageFileName, const std::string& maskFileName);

  std::string ObjectFileName;
  std::string ScanFileName;

  QFutureWatcher<void> FutureWatcher;
  QProgressDialog* ProgressDialog;
};

#endif // LidarProbabilityWidget_H
