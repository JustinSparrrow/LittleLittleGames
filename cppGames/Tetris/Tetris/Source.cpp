#include <Windows.h> //包含了一些用于Windows操作系统的函数和数据类型的声明。这个头文件提供了许多与窗口、消息处理、图形设备接口等相关的函数和结构体，可以用于开发Windows平台上的应用程序。
#include <thread>	//线程库
#include <vector>
#include <iostream>
using namespace std;

wstring tetromino[7];	//wstring表示宽字符字符串，可以存储和处理Unicode字符，输出时要用wcout
int nFieldWidth = 12;	//场宽
int nFieldHeight = 18;	//场高
unsigned char* pField = nullptr;	//无符号字符指针变量，可以用来指向一个无符号字符数组，用于表示游戏场地的状态

int nScreenWidth = 80;	// Console Screen Size X
int nScreenHeight = 30;	// Console Screen Size Y

//旋转函数
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

//当前方块是否可以放置在指定位置。参数：方块类型、方块的旋转状态、方块的位置坐标
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
	//一个俄罗斯方块为4*4
	tetromino[0].append(L"..X.");	//L""表示一个宽字符串，将"...."添加到tetromino[0]的末尾
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

	//初始化比赛场地数组
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;	//9表示边界，0表示空白

	//创建一个控制台屏幕缓冲区，并将其设置为活动屏幕缓冲区

	//创建了一个动态分配的wchar_r类型的数组，用于表示屏幕上的字符
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];	

	//将屏幕数组中的每个元素初始化为空格字符''
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

	//创建一个控制台屏幕缓冲区，并将其句柄存储在hConsole变量中
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	//将刚刚创建的控制台屏幕缓冲区设置为活动屏幕缓冲区，是的后续的输出将在该缓冲区中进行
	SetConsoleActiveScreenBuffer(hConsole);	

	//定义了变量用于存储写入缓冲区的字节数
	DWORD dwBytesWritten = 0;	

	//控制台屏幕缓冲区是一个用于存储文本和字符的内存区域，它在控制台窗口中显示文本和图形。它可以被看作是一个二维字符数组，每个字符都有一个对应的位置（行和列）
	//活动屏幕缓冲区是指当前正在被显示在控制台窗口中的屏幕缓冲区。在Windows操作系统中，控制台窗口可以有多个屏幕缓冲区，但只有一个缓冲区可以处于活动状态。活动屏幕缓冲区中的内容将被显示在控制台窗口中，用户可以看到和与其进行交互。

	//游戏循环
	// Game Logic Stuff
	bool bGameOver = false;

	int nCurrentPiece = 0;	//哪个俄罗斯方块正在下落
	int nCurrentRotation = 0;	//旋转了吗，旋转了多少角度
	int nCurrentX = nFieldWidth / 2;	//location
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotateHold = false;	//监测用户是否按住旋转按钮

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
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;	//R、L、D、Z

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
				//奖励机制，让游戏更上头的原因


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
				//if语句用来防止缓冲区溢出
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
			//此处在绘制图形时放置了游戏逻辑，此类方法并不常用
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
	/*将字符数组screen中的字符写入到控制台屏幕缓冲区中。
	* hConsole是之前创建的控制台屏幕缓冲区的句柄
	* screen是一个字符数组，包含要写入到屏幕缓冲区的字符
	* nScreenWidth*nScreenHeight表示要写入的字符数量，即屏幕缓冲区的大小
	* {0,0}是一个COORD结构，表示写入字符的起始位置，表示从屏幕缓冲区的左上角开始写入
	* &dwBytesWrittrn是一个指向DWORD类型的变量的指针，用于存储实际写入的字符数
	*/

	CloseHandle(hConsole);
	cout << "Game Over!! Score: " << nScore << endl;
	system("pause");
	
	return 0;
}
