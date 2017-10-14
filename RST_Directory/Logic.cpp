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
	/**
	* \brief Класс, представляющий управляющую логику приложения.
	* \details Реализует структуры для хранения и обработки данных.
	*/
	public ref class Logic
	{
	private:
		XmlDocument ^doc; ///< Поле, инкапсулирующее взаимодействие с xml.
	public:
		/**
		 * Реализует хранение нодов из XML документа.
		 * \param desc 24-символьный HEX ID объекта
		 * \param name Наименование объекта
		 */
		ref struct XMLData
		{
			property String^ desc; ///< Содержит ID объекта
			property String^ name; ///< Содержит наименование объекта
		};
		/**
		 * \brief Структура для хранения объекта с ID. Используется в промежуточном слое логики.
		 * \details Реализует интерфейс IEquatable.
		 */
		ref struct objectData : public IEquatable<objectData^>, public IEquatable<Object^>
		{
			String^ desc; ///< ID объекта
			String^ name; ///< Наименование объекта
			unsigned int count; ///< Количество экземпляров объекта

			///Реализация интерфейса IEquatable. Объекты считаются эквивалентными при идентичных [ID](@ref desc).
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
			/**
			 * Конструктор класса.
			 * \param d ID объекта
			 * \param n Наименование объекта
			 * \details Конструктор класса.
			 * Поле  `count` устанавливается равным 1.
			 */
			objectData(String^ d, String^ n)
			{
				desc = d;
				name = n;
				count = 1;
			}
		};
		/**
		* \brief Структура для хранения объекта. 
		* \details
		* Используется для представления данных в таблице. 
		* Реализует интерфейс IEquatable.
		*/
		ref struct gridData : public IEquatable<gridData^>, public IEquatable<Object^>
		{
			/**
			 * \param n Наименование объекта
			 * \details Свойство `count` устанавливается равным 1
			 */
			gridData(String^ n)
			{
				name = n;
				count = 1;
			}
			/**
			* \param n Наименование объекта
			* \param c Количество экземпляров объекта
			*/
			gridData(String^ n, unsigned int c)
			{
				name = n;
				count = c;
			}
			property String^ name; ///< Наименование объекта. Является свойством.
			property unsigned int count; ///< Количество экземпляров объекта. Является свойством.

			///Реализация интерфейса IEquatable. Объекты считаются эквивалентными при идентичных [name](@ref name).
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
		
		static Random^ rand;
		static ArrayList ^inData, ///< Список, хранящий [objectData](@ref objectData) элементы для добавления в таблицу приёма.
		^outData, ///< Список, хранящий [objectData](@ref objectData) элементы для добавления в таблицу отгрузки.
		^inGrid; ///< Список, хранящий данные таблицы приёма в экземплярах класса [gridData](@ref gridData).
		static ArrayList ^direct; ///< Список объектов, перечисленных в XML-файле.
		static ArrayList ^outGrid; ///< Список, хранящий данные таблицы отгрузки в экземплярах класса [gridData](@ref gridData).
		static ArrayList ^existed; ///< Список объектов с ID, полученными из поля ввода.
		/**
		 * Вызывается при старте программы.\n
		 *  Инициализирует необходимые списки, получает данные из xml файла.\n
		 * Записи в xml файле должны иметь вид
		 * \code
		 * <node desc="A7F1EAD42A8DDD13A76AE429" name="Salt" />
		 * \endcode
		 * и упакованы в тег `<objects>`.\n
		 * Полное содержимое  xml файла должно иметь следующую структуру:
		 * \code
		 * <?xml version="1.0" encoding="utf-8"?>
			<objects>
			<node desc="A7F1EAD42A8DDD13A76AE429" name="Salt" />
			<node desc="0B967E7BA0B50420817A3AC7" name="Spoon" />
			<node desc="CAF586559EF3EAB113C5EA33" name="Salt" />
			<node desc="53BF55E2BB3761A6F753D96B" name="Salt" />
			<node desc="16F1C27BB23A638112F79B12" name="Cookies" />
			<node desc="E2D0CCCF63FE0E8CCBF7C93C" name="Table" />
			</objects>
		 * 
		 * \endcode
		 * При отсутствии xml файла класс автоматически генерирует файл на 40 случайных записей.
		 */
		void onInit()
		{
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
			rand = gcnew Random();
			doc->CreateXmlDeclaration("1.0", "UTF-8", "no");
			auto root = doc->CreateElement("objects");
			ArrayList^ names = gcnew ArrayList();
			names->Add("Table");
			names->Add("Cup");
			names->Add("Bottle");
			names->Add("Salt");
			names->Add("Spoon");
			names->Add("Cookies");
			for (int i = 0; i < 40; i++)
			{
				root->AppendChild(next(RandomString(), (String^)names[rand->Next(0, names->Count)]));
			}
			delete names;
			delete rand;
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
		static String^ RandomString()
		{
			StringWriter^ builder = gcnew StringWriter();

			String^ pattern = "0123456789ABCDEF";
			char ch;
			for (int i = 0; i < 24; i++)
			{
				builder->Write(pattern[rand->Next(0, pattern->Length)]);
			}
			return builder->ToString();
		};
	};
}
