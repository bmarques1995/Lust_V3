#include "CloseController.hpp"

CloseController::CloseController(bool* signalEmmitter) :
	m_SignalEmmitter(signalEmmitter), m_CloseDialogOpened(false), m_CloseThread(nullptr)
{
}

CloseController::~CloseController()
{
}

void CloseController::ShowCloseDialog()
{
	m_CloseDialogOpened = true;
	m_CloseThread = new std::thread([this]() {
		OpenDialog();
	});
}

bool CloseController::IsDialogOpen() const
{
	return m_CloseDialogOpened;
}

void CloseController::CloseDialog()
{
	m_CloseDialogOpened = false;
}

void CloseController::OpenDialog()
{	
	m_Mutex.lock();
	*m_SignalEmmitter = true;
	m_Mutex.unlock();
}
