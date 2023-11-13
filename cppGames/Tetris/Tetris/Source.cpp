#include <Windows.h> //������һЩ����Windows����ϵͳ�ĺ������������͵����������ͷ�ļ��ṩ������봰�ڡ���Ϣ����ͼ���豸�ӿڵ���صĺ����ͽṹ�壬�������ڿ���Windowsƽ̨�ϵ�Ӧ�ó���
#include <thread>	//�߳̿�
#include <vector>
#include <iostream>
using namespace std;

/* ����˹���飨�ֿ���д��markdown�ļ��ڣ�
* 1.����˹���鹹�������飩
*	����˹������7����״������һ������ȥ��š��˴�ʹ���˿��ַ��������飬�ϸ������ϲ����Ƕ�ά���飬������Ҫ���������ά������﷨
*	���������⣺
*   ��1��ÿ������˹������4*4�ķ����ж���������ֲ�ͬ�Ĵ��ڷ�ʽ�����Ϊÿһ�ֶ���������̫��
*	��2��ȷ������˹����λ��ʱ����ͬ�İڷ�λ���в�ͬ����������������������Ҷ���˹����֧����ת��ʹ��ȷ��������������
*   ���������
*   ��1����Ϊһ��4*4�ķ��񣬴�������������������������֣����Եõ�a[x][y] = y*w���߿򳤣�+x��
*   ��2����תʱ����ÿ�������е�ֵ�Ʋ���ʽ���ɣ�˳ʱ�룩��
*		a.0�㣺i = y * w + x��
*		b.90�㣺i = 12 + y -��x * 4��
*		c.180�㣺i = 15 -��y * 4��- x
*		d.270�㣺i = 3 + y +��x * 4��
*		��ʱֻ��Ҫ����һ����ʽ�Ķ���˹����Ϳ��Ա�ʾ��������ʽ�ı��֡�
* 3.��������
*	�������ؽ���������������ͳ��ߣ�������Ϸ������߿�����
*	**<Windows.h>��ʹ��**����������ʾ��Ϸ���棬֮���Ƿ���Ըĳ�ʹ��QT
* 4.��Ϸѭ��
*	��Ϸѭ������Ϸ����������Ҫ�Ĳ��֣������ڷ�����һ�е�˳��
*	���ڶ���˹������˵����Ҫ���ģ���¼�����Ӧ�ã�ͨ������һЩ����ʱ��Ԫ�ء��û����롢������Ϸ�߼�Ȼ�����������Ļ��
*	��Ϸ��������н�������Ϸѭ��֮��
* 5.��Ϸ��������
*	��1��Game Timing����Ϸ��ʱ������һ�������ڿ�����Ϸ��ʱ�����š���ͨ����������֡�ʣ�FPS���͸�����Ϸ��ʱ�������ȷ����Ϸ�ڲ�ͬ���豸������ͬ���ٶ�����
*	��2��Input�����룩����һ������Ϸ�����ҵ����룬������̰��������������Ļ�����Ȳ�������Ϸ�������ҵ�������������һ���Ķ��������
*	��3��Game Logic����Ϸ�߼���������һ�����У���Ϸ��ִ����Ϸ���߼��͹��������������ײ��⡢������Ϸ״̬�����������������Ϸʱ��ȡ���Ϸ�߼���������Ϸ����Ϊ���淨
*	��4��Render Output����Ⱦ�����������һ�����У���Ϸ�Ὣ��Ϸ�����Ͷ�����Ⱦ����Ļ�ϣ���չʾ����ҡ������������Ϸ���桢��ɫ����������Ч�ȡ���Ⱦ���ʹ����ܹ�������Ϸ���Ӿ�Ч��
*	���ĸ�����ͨ������Ϸѭ����ѭ��ִ�У���ʵ����Ϸ�ĳ������кͽ����ԡ����ǹ�ͬ��������Ϸ�Ļ�����ܣ�ʹ��Ϸ�ܹ�������ҵ��������Ϸ�߼����и��º�չʾ
*/

wstring tetromino[7];	//wstring��ʾ���ַ��ַ��������Դ洢�ʹ���Unicode�ַ������ʱҪ��wcout
int nFieldWidth = 12;	//����
int nFieldHeight = 18;	//����
unsigned char* pField = nullptr;	//�޷����ַ�ָ���������������ָ��һ���޷����ַ����飬���ڱ�ʾ��Ϸ���ص�״̬

int nScreenWidth = 80;	// Console Screen Size X
int nScreenHeight = 30;	// Console Screen Size Y

//��ת����
int Rotate(int px, int py, int r) {
	switch (r % 4)
	{
	case 0: return py * 4 + px;			// 0 degrees
	case 1: return 12 + py - (px * 4);	// 90 degrees
	case 2: return 15 - (py * 4) - px;	// 180 degrees
	case 3: return 3 - py + (px * 4);	// 270 degrees
	default:
		break;
	}
	return 0;
}

//��ǰ�����Ƿ���Է�����ָ��λ�á��������������͡��������ת״̬�������λ������
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY) {
	for(int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++) {
			// Get index into piece
			int pi = Rotate(px, py, nRotation);

			// Get index into field
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth) {
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight) {
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
						return false;	//fail on first hit
				}
			}
		}

	return true;
}

int main() {
	// Create assets
	//һ������˹����Ϊ4*4
	tetromino[0].append(L"..X.");	//L""��ʾһ�����ַ�������"...."��ӵ�tetromino[0]��ĩβ
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	//��ʼ��������������
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;	//9��ʾ�߽磬0��ʾ�հ�

	//����һ������̨��Ļ������������������Ϊ���Ļ������

	//������һ����̬�����wchar_r���͵����飬���ڱ�ʾ��Ļ�ϵ��ַ�
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];	

	//����Ļ�����е�ÿ��Ԫ�س�ʼ��Ϊ�ո��ַ�''
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

	//����һ������̨��Ļ�����������������洢��hConsole������
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	//���ոմ����Ŀ���̨��Ļ����������Ϊ���Ļ���������ǵĺ�����������ڸû������н���
	SetConsoleActiveScreenBuffer(hConsole);	

	//�����˱������ڴ洢д�뻺�������ֽ���
	DWORD dwBytesWritten = 0;	

	//����̨��Ļ��������һ�����ڴ洢�ı����ַ����ڴ��������ڿ���̨��������ʾ�ı���ͼ�Ρ������Ա�������һ����ά�ַ����飬ÿ���ַ�����һ����Ӧ��λ�ã��к��У�
	//���Ļ��������ָ��ǰ���ڱ���ʾ�ڿ���̨�����е���Ļ����������Windows����ϵͳ�У�����̨���ڿ����ж����Ļ����������ֻ��һ�����������Դ��ڻ״̬�����Ļ�������е����ݽ�����ʾ�ڿ���̨�����У��û����Կ�����������н�����

	//��Ϸѭ��
	// Game Logic Stuff
	bool bGameOver = false;

	int nCurrentPiece = 0;	//�ĸ�����˹������������
	int nCurrentRotation = 0;	//��ת������ת�˶��ٽǶ�
	int nCurrentX = nFieldWidth / 2;	//location
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;	//����û��Ƿ�ס��ת��ť

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

	while (!bGameOver) {
		// Game Timing
		this_thread::sleep_for(50ms);	//Game Tick
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		// Input
		for (int k = 0; k < 4; k++)								
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;	//R��L��D��Z

		// Game Logic	=======================================
		//if (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) {nCurrentX = nCurrentX + 1;}
		//if (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) {nCurrentX = nCurrentX - 1;}
		//if (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {nCurrentY = nCurrentY + 1;}
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		if (bKey[3]) {
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = true;
		}
		else
			bRotateHold = false;

		if (bForceDown) {
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;	// It can go down
			else {
				// Lock the current piece into the field
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				nPieceCount++;
				if (nPieceCount % 10 == 0)
					if (nSpeed >= 10)nSpeed--;

				// check have wo got ant lines
				for(int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1) {
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

						if (bLine) {
							//Remove Line, set to = 
							for (int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;

							vLines.push_back(nCurrentY + py);
						}
					}

				nScore += 25;
				if (!vLines.empty()) nScore += (1 << vLines.size()) * 100;
				//�������ƣ�����Ϸ����ͷ��ԭ��


				// choose next piece
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				// if piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}

		// Render Output ========================================

		// Draw Field	=========================================
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++) {
				//if���������ֹ���������
				if (y + 2 < nScreenHeight && x + 2 < nScreenWidth) {
					screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];
				}
			}

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					if (py + 2 < nScreenHeight && px + 2 < nScreenWidth)
						screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		// Draw Current Piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					if (py + 2 < nScreenHeight && px + 2 < nScreenWidth)
						screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		// Show score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);


		if (!vLines.empty()) {
			// Display Frame (cheekily to draw lines)
			//�˴��ڻ���ͼ��ʱ��������Ϸ�߼������෽����������
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms);

			for (auto& v : vLines) {
				for (int px = 1; px < nFieldWidth - 1; px++) {
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					pField[px] = 0;
				}
			}
			vLines.clear();
		}

		// Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	/*���ַ�����screen�е��ַ�д�뵽����̨��Ļ�������С�
	* hConsole��֮ǰ�����Ŀ���̨��Ļ�������ľ��
	* screen��һ���ַ����飬����Ҫд�뵽��Ļ���������ַ�
	* nScreenWidth*nScreenHeight��ʾҪд����ַ�����������Ļ�������Ĵ�С
	* {0,0}��һ��COORD�ṹ����ʾд���ַ�����ʼλ�ã���ʾ����Ļ�����������Ͻǿ�ʼд��
	* &dwBytesWrittrn��һ��ָ��DWORD���͵ı�����ָ�룬���ڴ洢ʵ��д����ַ���
	*/

	CloseHandle(hConsole);
	cout << "Game Over!! Score: " << nScore << endl;
	system("pause");
	
	return 0;
}