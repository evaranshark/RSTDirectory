#include "MyForm.h"
//#include "Logic.cpp"

using namespace System;
using namespace System::Windows::Forms;
using namespace RST_Directory;


[STAThread]
void Main()
{
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	RST_Directory::MyForm form;
	Logic myLogic;
	myLogic.onInit();
	myLogic.form = %form;
	form.inSource->DataSource = Logic::inGrid;
	form.outSource->DataSource = Logic::outGrid;
	form.dataGridView1->DataSource = form.inSource;
	form.dataGridView2->DataSource = form.outSource;
	Logic::outGrid->Add(gcnew Logic::gridData("Name2", 1));
	
	Application::Run(%form);
}
