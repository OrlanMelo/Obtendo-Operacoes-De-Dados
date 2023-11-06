#include <Windows.h>
#include <ioapiset.h>
#include <string>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#pragma comment(lib,"kernel32.lib")

using namespace std;

class cFuncoes
{
private:

	DISK_PERFORMANCE Performance;
	HANDLE Identificador;

	bool ObterInformacoesDeDesempenho(wstring Unidade)
	{
		bool bRetorno = false;

		Identificador = CreateFile(Unidade.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if (Identificador == INVALID_HANDLE_VALUE)
		{
			cout << "Identificador inválido para a unidade selecionada..\n";
		}
		else
		{
			DWORD BytesRetornados;

			//Esta função retorna TRUE após sucesso e FALSE caso contrário, poderá inserir as condições, caso seja necessário.
			DeviceIoControl(Identificador, IOCTL_DISK_PERFORMANCE, 0, 0, &Performance, sizeof(Performance), &BytesRetornados, 0);

			bRetorno = true;
		}

		return bRetorno;

	}

	IO_COUNTERS Processo;

public:

	void ObterDados(wstring Unidade)
	{
		if (ObterInformacoesDeDesempenho(Unidade) == true)
		{
			
			LONGLONG CalcLeituraMB = (double)Performance.BytesRead.QuadPart / (1024 * 1024);
			//LONGLONG CalcLeituraGB = (double)Performance.BytesRead.QuadPart / (1024 * 1024 * 1024);

			LONGLONG CalcGravacaoMB = (double)Performance.BytesWritten.QuadPart / (1024 * 1024);
			//LONGLONG CalcGravacaoGB = (double)Performance.BytesWritten.QuadPart / (1024 * 1024 * 1024);

			cout << "Informações do disco selecionado..\n";

			_tprintf(L"Gerenciador: %s\n", Performance.StorageManagerName);

			cout << "Identificador: " << Performance.StorageDeviceNumber << endl;

			cout << "Leituras p/segundo..: " << CalcLeituraMB << " MB/s" << endl;
			cout << "Operações de leitura: " << Performance.ReadCount << " de leituras..\n";

			cout << "Gravações p/segundo..: " << CalcGravacaoMB << " MB/s" << endl;
			cout << "Operações de gravação: " << Performance.WriteCount << " de gravações..\n";

			cout << "Contagem de E/S fragmentadas: " << Performance.SplitCount << endl;
		}
	}

	void DadosDeProcesso(HANDLE Id)
	{
		GetProcessIoCounters(Id, &Processo);
		
		cout << "Obtendo informações de desempenho do processo selecionado..\n\n";

		cout << "Operações executadas: " << Processo.ReadOperationCount << " de leituras..\n";

		ULONGLONG CalcMB = (double)Processo.ReadTransferCount / (1024 * 1024);
		cout << "Leituras feitas p/segundo: " << CalcMB << " MB/s..\n";

		cout << "Operações executadas: " << Processo.WriteOperationCount << " de gravações..\n";
		
		ULONGLONG CalcMB2 = (double)Processo.WriteTransferCount / (1024 * 1024);
		cout << "Gravações p/segundo: " << CalcMB2 << " MB/s..\n";

		cout << "Outras operações de E/S executadas: " << Processo.OtherOperationCount << " operações, exceto leitura e gravação..\n";

		ULONGLONG CalcMB3 = (double)Processo.OtherTransferCount / (1024 * 1024);
		cout << "Outras operações diferentes de leitura e gravação: " << CalcMB3 << " em MBs\n";
	}

	void FecharIdentificador()
	{
		CloseHandle(Identificador);
	}

}Funcoes;

int main()
{

	cout << "O assistente está executando operações de leitura de dados de E/S...\n\n";

	Funcoes.ObterDados(L"\\\\.\\PhysicalDrive0");//Meu disco local C.

	/*
	* Usamos GetCurrentProcess para obter o identificador do processo atual.
	* Poderá implementar para obter operações de E/S de outros processos.
	*/
	Funcoes.DadosDeProcesso(GetCurrentProcess());

	Funcoes.FecharIdentificador();

	system("pause");
}
