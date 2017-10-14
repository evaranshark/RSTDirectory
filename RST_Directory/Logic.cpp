#pragma once
#include <list>
#include <string>

using namespace std;
using namespace System::Windows::Forms;
using namespace System::Collections;
using namespace System;
using namespace System::Xml;
using namespace System::IO;
using namespace System;


namespace RST_Directory {
	public ref class Logic
	{
	public:
		ref struct XMLData
		{
			property String^ desc;
			property String^ name;
		};
		ref struct objectData : public IEquatable<objectData^>, public IEquatable<Object^>
		{
			String^ desc;
			String^ name;
			unsigned int count;
			bool Equals(objectData^ right) override
			{
				return desc == right->desc;
			}
			virtual bool Equals(Object^ right) override
			{
				return desc->GetHashCode() == ((objectData^)right)->desc->GetHashCode();
			}
			virtual int GetHashCode(objectData^ data)
			{
				return data->desc->GetHashCode();
			}
			objectData(String^ d, String^ n)
			{
				desc = d;
				name = n;
				count = 1;
			}
		};

		ref struct gridData : public IEquatable<gridData^>, public IEquatable<Object^>
		{
			gridData(String^ n)
			{
				name = n;
				count = 1;
			}
			gridData(String^ n, unsigned int c)
			{
				name = n;
				count = c;
			}
			property String^ name;
			property unsigned int count;
			bool Equals(gridData^ right) override
			{
				return name == right->name;
			}
			virtual bool Equals(Object^ right) override
			{
				return name->GetHashCode() == ((gridData^)right)->name->GetHashCode();
			}
			virtual int GetHashCode(gridData^ data)
			{
				return data->name->GetHashCode();
			}
		};
	
		
		static Form^ form;
		static ArrayList ^inData, ^outData, ^inGrid;
		static ArrayList^ direct;
		static ArrayList^ outGrid;
		static ArrayList^ existed;
		XmlDocument^ doc;
	
		void onInit()
		{
			int state;
			doc = gcnew XmlDocument();
			StreamReader^ xmlReader;
			try 
			{
				xmlReader = gcnew StreamReader("direct.xml");
			}
			catch (FileNotFoundException^ e)
			{
				createDir();
				xmlReader = gcnew StreamReader("direct.xml");
			}
			doc->Load(xmlReader);	
			direct = gcnew ArrayList();
			parseDir();
			xmlReader->Close();
			outGrid = gcnew ArrayList();
			inGrid = gcnew ArrayList();
			inData = gcnew ArrayList();
			outData = gcnew ArrayList();
		}
		void createDir()
		{
			doc->CreateXmlDeclaration("1.0", "UTF-8", "no");
			auto root = doc->CreateElement("objects");
			root->AppendChild(next("AAAAAAAAAAAAAAAAAAAAAAAA", "Object A"));
			root->AppendChild(next("AAAAAAAAAAAAAAAAAAAAAAAB", "Object B"));
			root->AppendChild(next("CAAAAAAAAAAAAAAAAAAAAAAA", "Object 3"));
			root->AppendChild(next("DAAAAAAAAAAAAAAAAAAAAAAA", "Object 4"));
			root->AppendChild(next("AAAAAAAAAAAAAAAAAAAAAAA1", "Object A"));
			root->AppendChild(next("AAAAAAAAAAAAAAAAAAAAAAA2", "Object 2"));
			root->AppendChild(next("AAAAAAAAAAAAAAAAAAAAAAA3", "Object 3"));
			doc->AppendChild(root);
			StreamWriter^ xmlWriter = gcnew StreamWriter("direct.xml");
			doc->Save(xmlWriter);
			xmlWriter->Close();
		}
		void parseDir()
		{
			auto root = doc->FirstChild->NextSibling;
			auto end = root->LastChild;
			auto element = root->FirstChild;
			XMLData^ data;
			do
			{
				data = gcnew XMLData;
				data->desc = element->Attributes["desc"]->Value;
				data->name = element->Attributes["name"]->Value;
				direct->Add(data);
				element = element->NextSibling;
			} while (element != end);
			data = gcnew XMLData;
			data->desc = element->Attributes["desc"]->Value;
			data->name = element->Attributes["name"]->Value;
			direct->Add(data);
		}
		
		XmlElement^ next(String^ desc, String^ name)
		{
			XmlElement^ result = doc->CreateElement("node");
			result->SetAttribute("desc", desc);
			result->SetAttribute("name", name);
			return result;
		}
		static System::Void inBoxEnter(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
		{
			TextBox^ tBox = (TextBox^)sender;
			if (e->KeyCode == Keys::Enter)
			{
				auto tBoxValue = tBox->Text->ToString();
				auto values = tBoxValue->Split(' ');
				ArrayList^ vals = gcnew ArrayList();
				for each (auto val in values)
					vals->Add(val);
				delete values;
				FindExisted(vals);
				if (tBox->Name == L"tBoxIn")
				{
					UpdateData(true);
				}
				else if (tBox->Name == L"tBoxOut")
				{
					UpdateData(false);
				}
				UpdateGrid();
				tBox->Clear();
			}
		}

		static void FindExisted(ArrayList^ values)
		{
			existed = gcnew ArrayList();
			for each (String^ val in values)
			for each (XMLData^ tuple in direct)
			{
				if (tuple->desc == val)
				{
					existed->Add(tuple);
					break;
				}
			}
		}
		static void UpdateData(bool flag)
		{
			if (flag)
			{
				for each (XMLData^ ex in existed)
				{
					if (!inData->Contains(gcnew objectData(ex->desc, ex->name)))
						inData->Add(gcnew objectData(ex->desc, ex->name));
					else
					{
						((objectData^)inData[inData->IndexOf(gcnew objectData(ex->desc, ex->name))])->count++;
					}
				}
				for each (XMLData^ ex in existed)
				{
					if (outData->Contains(gcnew objectData(ex->desc, ex->name)))
					{
						int index = outData->IndexOf(gcnew objectData(ex->desc, ex->name));
						((objectData^)outData[index])->count--;
						if (((objectData^)outData[index])->count == 0)
							outData->RemoveAt(index);
					}
				}
			}
			else
			{
				for each (XMLData^ ex in existed)
				{
					if (!outData->Contains(gcnew objectData(ex->desc, ex->name)))
						outData->Add(gcnew objectData(ex->desc, ex->name));
					else
					{
						((objectData^)outData[outData->IndexOf(gcnew objectData(ex->desc, ex->name))])->count++;
					}
				}
				for each (XMLData^ ex in existed)
				{
					if (inData->Contains(gcnew objectData(ex->desc, ex->name)))
					{
						int index = inData->IndexOf(gcnew objectData(ex->desc, ex->name));
						((objectData^)inData[index])->count--;
						if (((objectData^)inData[index])->count == 0)
							inData->RemoveAt(index);
					}
				}
			}
		}
		static void UpdateGrid()
		{
			inGrid->Clear();
			outGrid->Clear();
			for each (objectData^ el in inData)
			{
				if (!inGrid->Contains(gcnew gridData(el->name)))
					inGrid->Add(gcnew gridData(el->name, el->count));
				else
				{
					((gridData^)inGrid[inGrid->IndexOf(gcnew gridData(el->name))])->count += el->count;
				}
			}
			for each (objectData^ el in outData)
			{
				if (!outGrid->Contains(gcnew gridData(el->name)))
					outGrid->Add(gcnew gridData(el->name, el->count));
				else
				{
					((gridData^)outGrid[outGrid->IndexOf(gcnew gridData(el->name))])->count += el->count;
				}
			}
		}
	};
}
