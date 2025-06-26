#pragma once

#include <thread>
#include <mutex>
#include <QMessageBox>

class CloseController
{
public:
	CloseController(bool* signalEmmitter);
	~CloseController();

	void ShowCloseDialog();
	bool IsDialogOpen() const;
	void CloseDialog();

private:
	void OpenDialog();

	bool* m_SignalEmmitter;

	std::thread* m_CloseThread;
	std::mutex m_Mutex;
	bool m_CloseDialogOpened;

};