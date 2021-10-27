#include "CGlobal.h"

#include <QtWidgets/QApplication>
#include <QtGui/QFontDatabase>

static CGlobal g_SxGlobal;
CGlobal* GetGlobalPtr() {
	return &g_SxGlobal;
}

CGlobal::CGlobal() {
}
CGlobal::~CGlobal() {

}


